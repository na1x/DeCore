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
    : WaitPlayer(syncData, moveCount)
{

}

const Card& SaveRestoreTest::AttackWaitPlayer::attack(const PlayerId* playerId, const CardSet& cardSet)
{
    processMove();
    return WaitPlayer::attack(playerId, cardSet);
}

const Card* SaveRestoreTest::AttackWaitPlayer::pitch(const PlayerId* playerId, const CardSet& cardSet)
{
    processMove();
    return WaitPlayer::pitch(playerId, cardSet);
}

SaveRestoreTest::WaitPlayer::WaitPlayer(PlayerSyncData& syncData, unsigned int moveCount)
    : mSyncData(syncData)
    , mMoveCount(moveCount)
{

}

void SaveRestoreTest::WaitPlayer::processMove()
{
    assert(mMoveCount);
    mMoveCount--;
    // notify only when mMoveCount moves performed
    if (!mMoveCount) {
        mSyncData.signalThread();
        mSyncData.waitForMove();
    }
}

SaveRestoreTest::DefendWaitPlayer::DefendWaitPlayer(PlayerSyncData& syncData, unsigned int moveCount)
    : WaitPlayer(syncData, moveCount)
{
}

const Card* SaveRestoreTest::DefendWaitPlayer::defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet)
{
    processMove();
    return WaitPlayer::defend(playerId, attackCard, cardSet);
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

    GameCardsTracker tracker;
    Observer observer;

    engine.addGameObserver(tracker);
    engine.addGameObserver(observer);

    Deck deck;

    generate(deck);

    engine.setDeck(deck);

    threadData.mSyncData.setEngine(&engine);

    while (engine.playRound());

    return NULL;
}

void SaveRestoreTest::test(Player& player0, Player& player1, Player& restoredPlayer0, Player& restoredPlayer1, PlayerSyncData& syncData,
                           GameCardsTracker& restoredTracker, Engine& restored, Observer& restoredObserver) {
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
    observers.push_back(&restoredObserver);

    TestReader reader(savedData.mBytes);
    restored.init(reader, restoredPlayers, observers);

    CPPUNIT_ASSERT(reader.mByteIndex == savedData.mBytes.size());

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

void SaveRestoreTest::testDataReader()
{
    TestWriter writer;
    TestReader reader(writer.mBytes);
    const unsigned int a = 10;
    const unsigned char b = 'b';
    Deck deck;
    generate(deck);
    writer.write(a);
    writer.write(b);
    writer.write(deck.begin(), deck.end());
    unsigned int readA;
    unsigned char readB;
    Deck readDeck;
    reader.read(readA);
    reader.read(readB);
    reader.read(readDeck, Card(SUIT_LAST, RANK_LAST));
    CPPUNIT_ASSERT(a == readA);
    CPPUNIT_ASSERT(b == readB);
    CPPUNIT_ASSERT(deck == readDeck);
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
    Observer observer;

    test(player0, player1, restoredPlayer0, restoredPlayer1, syncData, tracker, restored, observer);

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
    Observer observer;

    test(player0, player1, restoredPlayer0, restoredPlayer1, syncData, tracker, restored, observer);

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
    // attacker sleeps on third attack move
    // save game while attacker waits and save
    // restore the game and check
    PlayerSyncData syncData;
    AttackWaitPlayer player0(syncData, 3);
    BasePlayer player1;

    BasePlayer restoredPlayer0, restoredPlayer1;
    Engine restored;
    GameCardsTracker tracker;
    Observer observer;

    test(player0, player1, restoredPlayer0, restoredPlayer1, syncData, tracker, restored, observer);

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

void SaveRestoreTest::test03()
{
    // defender sleeps on first defend move
    // save game while attacker waits and save
    // restore the game and check
    PlayerSyncData syncData;
    BasePlayer player0;
    DefendWaitPlayer player1(syncData, 1);

    BasePlayer restoredPlayer0, restoredPlayer1;
    Engine restored;
    GameCardsTracker tracker;
    Observer observer;

    test(player0, player1, restoredPlayer0, restoredPlayer1, syncData, tracker, restored, observer);

    CPPUNIT_ASSERT(MAX_CARDS - 1 == tracker.playerCards(restoredPlayer0.id()).unknownCards()); // attack done
    CPPUNIT_ASSERT(MAX_CARDS == tracker.playerCards(restoredPlayer1.id()).unknownCards());

    CPPUNIT_ASSERT(2 == restoredPlayer0.cardSets());
    CPPUNIT_ASSERT(1 == restoredPlayer1.cardSets());

    CPPUNIT_ASSERT(restoredPlayer0.cards(restoredPlayer0.cardSets() - 1).size() == MAX_CARDS - 1);
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

void SaveRestoreTest::test04()
{
    // defender sleeps on first defend move
    // save game while attacker waits and save
    // restore the game and check
    PlayerSyncData syncData;
    BasePlayer player0;
    DefendWaitPlayer player1(syncData, 2);

    BasePlayer restoredPlayer0, restoredPlayer1;
    Engine restored;
    GameCardsTracker tracker;
    Observer observer;

    test(player0, player1, restoredPlayer0, restoredPlayer1, syncData, tracker, restored, observer);

    CPPUNIT_ASSERT(MAX_CARDS - 2 == tracker.playerCards(restoredPlayer0.id()).unknownCards()); // attack done
    CPPUNIT_ASSERT(MAX_CARDS - 1 == tracker.playerCards(restoredPlayer1.id()).unknownCards());

    CPPUNIT_ASSERT(3 == restoredPlayer0.cardSets());
    CPPUNIT_ASSERT(2 == restoredPlayer1.cardSets());

    CPPUNIT_ASSERT(restoredPlayer0.cards(restoredPlayer0.cardSets() - 1).size() == MAX_CARDS - 2);
    CPPUNIT_ASSERT(restoredPlayer1.cards(restoredPlayer1.cardSets() - 1).size() == MAX_CARDS - 1);

    CPPUNIT_ASSERT(tracker.attackCards().size() == 2);
    CPPUNIT_ASSERT(tracker.defendCards().size() == 1);

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
    CPPUNIT_ASSERT(mByteIndex < mBytes.size());
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

