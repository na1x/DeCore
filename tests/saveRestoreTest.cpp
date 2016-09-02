#include <cassert>

#include "saveRestoreTest.h"
#include "engine.h"
#include "deck.h"
#include "card.h"
#include "gameTest.h"
#include "gameCardsTracker.h"
#include "defines.h"

using namespace decore;

const unsigned int SaveRestoreTest::MAX_CARDS = 6;

SaveRestoreTest::ThreadData::ThreadData(Player& player0, Player& player1, PlayerSyncData& syncData)
    : mPlayer0(player0)
    , mPlayer1(player1)
    , mSyncData(syncData)
{

}


SaveRestoreTest::AttackWaitPlayer::AttackWaitPlayer(PlayerSyncData& syncData, unsigned int moveCount)
    : mSyncData(syncData)
    , mMoveCount(moveCount)
{

}

const Card& SaveRestoreTest::AttackWaitPlayer::attack(const PlayerId* playerId, const CardSet& cardSet)
{
    processMove();
    return BasePlayer::attack(playerId, cardSet);
}

const Card* SaveRestoreTest::AttackWaitPlayer::pitch(const PlayerId* playerId, const CardSet& cardSet)
{
    processMove();
    return BasePlayer::pitch(playerId, cardSet);
}

void SaveRestoreTest::AttackWaitPlayer::processMove()
{
    assert(mMoveCount);
    mMoveCount--;
    // notify only when mMoveCount moves performed
    if (!mMoveCount) {
        mSyncData.signalThread();
        mSyncData.waitForMove();
    }
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

void* SaveRestoreTest::testThread(void* data)
{
    ThreadData& threadData = *static_cast<ThreadData*>(data);

    Engine engine;

    engine.add(threadData.mPlayer0);
    engine.add(threadData.mPlayer1);

    GameCardsTracker observer;

    engine.addGameObserver(observer);

    Deck deck;

    generate(deck);

    engine.setDeck(deck);

    threadData.mSyncData.setEngine(&engine);

    while (engine.playRound());

    return NULL;
}

void SaveRestoreTest::test(Player& player0, Player& player1, Player& restoredPlayer0, Player& restoredPlayer1, PlayerSyncData& syncData, GameCardsTracker& restoredTracker, Engine& restored) {
    // start game in separate thread
    // save the game and terminate
    // create new game from saved data
    // validate created instance

    pthread_t engineThread;

    ThreadData threadData(player0, player1, syncData);

    pthread_create(&engineThread, NULL, testThread, &threadData);

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

    // create new engine from the saved data and validate
    std::vector<Player*> restoredPlayers;
    restoredPlayers.push_back(&restoredPlayer0);
    restoredPlayers.push_back(&restoredPlayer1);

    std::vector<GameObserver*> observers;
    observers.push_back(&restoredTracker);

    TestReader reader(savedData.mBytes);
    restored.init(reader, restoredPlayers, observers);

    CPPUNIT_ASSERT(0 == restoredTracker.lastRoundIndex());
    // check deck size
    CPPUNIT_ASSERT(24 == restoredTracker.deckCards());

    Deck deck;
    generate(deck);
    CardSet expectedDeck;
    expectedDeck.addAll(deck);

    CPPUNIT_ASSERT(expectedDeck == restoredTracker.gameCards());
    CPPUNIT_ASSERT(SUIT_CLUBS == restoredTracker.trumpSuit());
}

void SaveRestoreTest::checkNoDeal(std::vector<BasePlayer*> players)
{
    // check that there was no deal and amount of player cards always decremented
    for (std::vector<BasePlayer*>::iterator it = players.begin(); it != players.end(); ++it) {
        BasePlayer& player = **it;
        unsigned int maxCards = -1;
        for (unsigned int i = 0; i < player.cardSets(); i++) {
            CPPUNIT_ASSERT(maxCards > player.cards(i).size());
            maxCards = player.cards(i).size();
        }
    }
}

void SaveRestoreTest::test00()
{
    // attacker sleeps on first attack move
    // save game while attacker waits and save
    // restore the game and check
    PlayerSyncData syncData;
    AttackWaitPlayer player0(syncData, 1);
    BasePlayer player1;

    BasePlayer restoredPlayer0, restoredPlayer1;
    Engine restored;
    GameCardsTracker tracker;

    test(player0, player1, restoredPlayer0, restoredPlayer1, syncData, tracker, restored);

    for (PlayerIds::const_iterator it = tracker.playerIds().begin(); it != tracker.playerIds().end(); ++it) {
        const PlayerCards& playerCards = tracker.playerCards(*it);
        CPPUNIT_ASSERT(MAX_CARDS == playerCards.unknownCards()); // no moves done yet
    }

    CPPUNIT_ASSERT(1 == restoredPlayer0.cardSets());
    CPPUNIT_ASSERT(1 == restoredPlayer1.cardSets());

    CPPUNIT_ASSERT(restoredPlayer0.cards(restoredPlayer0.cardSets() - 1).size() == MAX_CARDS);
    CPPUNIT_ASSERT(restoredPlayer1.cards(restoredPlayer1.cardSets() - 1).size() == MAX_CARDS);

    CPPUNIT_ASSERT(36 == tracker.gameCards().size());

    // continue game
    CPPUNIT_ASSERT(restored.playRound());

    std::vector<BasePlayer*> restoredPlayers;
    restoredPlayers.push_back(&restoredPlayer0);
    restoredPlayers.push_back(&restoredPlayer1);

    checkNoDeal(restoredPlayers);

    CPPUNIT_ASSERT(6 == tracker.goneCards().size());
}

void SaveRestoreTest::test01()
{
    // attacker sleeps on second attack move
    // save game while attacker waits and save
    // restore the game and check
    PlayerSyncData syncData;
    AttackWaitPlayer player0(syncData, 2);
    BasePlayer player1;

    BasePlayer restoredPlayer0, restoredPlayer1;
    Engine restored;
    GameCardsTracker tracker;

    test(player0, player1, restoredPlayer0, restoredPlayer1, syncData, tracker, restored);

    for (PlayerIds::const_iterator it = tracker.playerIds().begin(); it != tracker.playerIds().end(); ++it) {
        const PlayerCards& playerCards = tracker.playerCards(*it);
        CPPUNIT_ASSERT(MAX_CARDS - 1 == playerCards.unknownCards()); // no moves done yet
    }

    CPPUNIT_ASSERT(2 == restoredPlayer0.cardSets());
    CPPUNIT_ASSERT(2 == restoredPlayer1.cardSets());

    CPPUNIT_ASSERT(restoredPlayer0.cards(restoredPlayer0.cardSets() - 1).size() == MAX_CARDS - 1);
    CPPUNIT_ASSERT(restoredPlayer1.cards(restoredPlayer1.cardSets() - 1).size() == MAX_CARDS - 1);

    CPPUNIT_ASSERT(36 == tracker.gameCards().size());

    // continue game
    CPPUNIT_ASSERT(restored.playRound());

    std::vector<BasePlayer*> restoredPlayers;
    restoredPlayers.push_back(&restoredPlayer0);
    restoredPlayers.push_back(&restoredPlayer1);

    checkNoDeal(restoredPlayers);

    CPPUNIT_ASSERT(6 == tracker.goneCards().size());
}

void SaveRestoreTest::test02()
{
    // attacker sleeps on second attack move
    // save game while attacker waits and save
    // restore the game and check
    PlayerSyncData syncData;
    AttackWaitPlayer player0(syncData, 3);
    BasePlayer player1;

    BasePlayer restoredPlayer0, restoredPlayer1;
    Engine restored;
    GameCardsTracker tracker;

    test(player0, player1, restoredPlayer0, restoredPlayer1, syncData, tracker, restored);

    for (PlayerIds::const_iterator it = tracker.playerIds().begin(); it != tracker.playerIds().end(); ++it) {
        const PlayerCards& playerCards = tracker.playerCards(*it);
        CPPUNIT_ASSERT(MAX_CARDS - 2 == playerCards.unknownCards()); // no moves done yet
    }

    CPPUNIT_ASSERT(3 == restoredPlayer0.cardSets());
    CPPUNIT_ASSERT(3 == restoredPlayer1.cardSets());

    CPPUNIT_ASSERT(restoredPlayer0.cards(restoredPlayer0.cardSets() - 1).size() == MAX_CARDS - 2);
    CPPUNIT_ASSERT(restoredPlayer1.cards(restoredPlayer1.cardSets() - 1).size() == MAX_CARDS - 2);

    CPPUNIT_ASSERT(36 == tracker.gameCards().size());

    // continue game
    CPPUNIT_ASSERT(restored.playRound());

    std::vector<BasePlayer*> restoredPlayers;
    restoredPlayers.push_back(&restoredPlayer0);
    restoredPlayers.push_back(&restoredPlayer1);

    checkNoDeal(restoredPlayers);

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

void SaveRestoreTest::generate(Deck& deck)
{
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
}

