#include <stdlib.h>
#include <cassert>
#include <iomanip>

#include "saveRestoreTest.h"
#include "engine.h"
#include "deck.h"
#include "card.h"
#include "gameTest.h"
#include "gameCardsTracker.h"
#include "defines.h"

using namespace decore;

const unsigned int SaveRestoreTest::MAX_CARDS = 6;

SaveRestoreTest::AttackWaitPlayer::AttackWaitPlayer(PlayerSyncData& syncData)
    : mSyncData(syncData)
{

}

const Card& SaveRestoreTest::AttackWaitPlayer::attack(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
    mSyncData.signalThread();
    mSyncData.waitForMove();
    return *cardSet.begin();
}

SaveRestoreTest::DefendWaitPlayer::DefendWaitPlayer(PlayerSyncData& syncData)
    : mSyncData(syncData)
{
}

const Card* SaveRestoreTest::DefendWaitPlayer::defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet)
{
    (void) playerId;
    (void) attackCard;
    (void) cardSet;
    mSyncData.waitForMove();
    return NULL;
}

SaveRestoreTest::PlayerSyncData::PlayerSyncData()
    : mDontWaitForMove(false)
    , mEngine(NULL)
{
    pthread_mutex_init(&mMoveMutex, NULL);
    pthread_cond_init(&mMoveSignal, NULL);

    pthread_mutex_init(&mThreadMutex, NULL);
    pthread_cond_init(&mThreadSignal, NULL);
}

SaveRestoreTest::PlayerSyncData::~PlayerSyncData()
{
    pthread_mutex_destroy(&mMoveMutex);
    pthread_cond_destroy(&mMoveSignal);

    pthread_mutex_destroy(&mThreadMutex);
    pthread_cond_destroy(&mThreadSignal);
}

void SaveRestoreTest::PlayerSyncData::signalMove()
{
    pthread_mutex_lock(&mMoveMutex);
    mDontWaitForMove = true;
    pthread_cond_broadcast(&mMoveSignal);
    pthread_mutex_unlock(&mMoveMutex);
}

void SaveRestoreTest::PlayerSyncData::waitForMove()
{
    pthread_mutex_lock(&mMoveMutex);
    if (!mDontWaitForMove) {
        pthread_cond_wait(&mMoveSignal, &mMoveMutex);
    }
    pthread_mutex_unlock(&mMoveMutex);
}

void SaveRestoreTest::PlayerSyncData::setEngine(Engine* engine)
{
    pthread_mutex_lock(&mThreadMutex);
    mEngine = engine;
    pthread_mutex_unlock(&mThreadMutex);
}

void SaveRestoreTest::PlayerSyncData::signalThread()
{
    pthread_mutex_lock(&mThreadMutex);
    assert(mEngine);
    pthread_cond_broadcast(&mThreadSignal);
    pthread_mutex_unlock(&mThreadMutex);
}

Engine* SaveRestoreTest::PlayerSyncData::waitForThread()
{
    pthread_mutex_lock(&mThreadMutex);
    if (!mEngine) {
        pthread_cond_wait(&mThreadSignal, &mThreadMutex);
    }
    pthread_mutex_unlock(&mThreadMutex);
    return mEngine;
}

void* SaveRestoreTest::attackWaitTestThread(void* data)
{
    PlayerSyncData& syncData = *static_cast<PlayerSyncData*>(data);

    Engine engine;

    AttackWaitPlayer player0(syncData);
    BasePlayer player1;

    engine.add(player0);
    engine.add(player1);

    GameCardsTracker observer;

    engine.addGameObserver(observer);

    Deck deck;

    Rank ranks[] = {
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_9,
        RANK_10,
        RANK_JACK,
        RANK_QUEEN,
        RANK_KING,
        RANK_ACE,
    };

    Suit suits[] = {
        SUIT_SPADES,
        SUIT_HEARTS,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };

    deck.generate(ranks, ARRAY_SIZE(ranks), suits, ARRAY_SIZE(suits));

    engine.setDeck(deck);

    syncData.setEngine(&engine);

    engine.playRound();

    return NULL;
}

void SaveRestoreTest::test00()
{
    // start game in separate thread
    // save the game and terminate
    // create new game from saved data
    // validate created instance

    pthread_t engineThread;

    PlayerSyncData syncData;

    pthread_create(&engineThread, NULL, attackWaitTestThread, &syncData);

    Engine* engine = syncData.waitForThread();
    CPPUNIT_ASSERT(engine);

    // at this point engine is created and round started
    // game flow will stuck on player's one attack
    // save data here
    TestWriter savedData;
    engine->save(savedData);
    // request quit
    engine->quit();
    // ping player one to return from "attack" method

    syncData.signalMove();

    pthread_join(engineThread, NULL);

    CPPUNIT_ASSERT(!savedData.mBytes.empty());

    Engine restored;
    BasePlayer player0, player1;
    std::vector<Player*> players;
    players.push_back(&player0);
    players.push_back(&player1);

    GameCardsTracker tracker;

    std::vector<GameObserver*> observers;
    observers.push_back(&tracker);

    TestReader reader(savedData.mBytes);
    restored.init(reader, players, observers);

    CPPUNIT_ASSERT(0 == tracker.lastRoundIndex());
    // check deck size
    CPPUNIT_ASSERT(24 == tracker.deckCards());

    for (PlayerIds::const_iterator it = tracker.playerIds().begin(); it != tracker.playerIds().end(); ++it) {
        const PlayerCards& playerCards = tracker.playerCards(*it);
        CPPUNIT_ASSERT(MAX_CARDS == playerCards.unknownCards()); // no moves done yet
    }

    Deck deck;

    Rank ranks[] = {
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_9,
        RANK_10,
        RANK_JACK,
        RANK_QUEEN,
        RANK_KING,
        RANK_ACE,
    };

    Suit suits[] = {
        SUIT_SPADES,
        SUIT_HEARTS,
        SUIT_DIAMONDS,
        SUIT_CLUBS,
    };

    deck.generate(ranks, ARRAY_SIZE(ranks), suits, ARRAY_SIZE(suits));

    CardSet expectedDeck;
    expectedDeck.addAll(deck);

    CPPUNIT_ASSERT(36 == tracker.gameCards().size());
    CPPUNIT_ASSERT(expectedDeck == tracker.gameCards());
    CPPUNIT_ASSERT(SUIT_CLUBS == tracker.trumpSuit());

    // continue game
    CPPUNIT_ASSERT(restored.playRound());

    CPPUNIT_ASSERT(6 == tracker.goneCards().size());
}

void SaveRestoreTest::TestWriter::write(const void* data, unsigned int dataSizeBytes)
{
    const unsigned char* dataPtr = static_cast<const unsigned char*>(data);
    assert(dataSizeBytes);
    mBytes.push_back(dataSizeBytes);
    while (dataSizeBytes--) {
        mBytes.push_back(*dataPtr++);
    }
}

unsigned int SaveRestoreTest::TestWriter::position() const
{
    return mBytes.size();
}

SaveRestoreTest::TestReader::TestReader(const std::vector<unsigned char>& bytes)
    : mByteIndex(0)
    , mBytes(bytes)
{

}

unsigned int SaveRestoreTest::TestReader::position() const
{
    return mByteIndex;
}

void SaveRestoreTest::TestReader::read(void* data, unsigned int dataSizeBytes)
{
    unsigned char* dataPtr = static_cast<unsigned char*>(data);
    assert(dataSizeBytes);
    unsigned char recordedDataSize = mBytes[mByteIndex++];
    CPPUNIT_ASSERT(dataSizeBytes == recordedDataSize);
    while (dataSizeBytes--) {
        CPPUNIT_ASSERT(mByteIndex < mBytes.size());
        *dataPtr++ = mBytes[mByteIndex++];
    }
}
