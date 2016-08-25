#include <stdlib.h>

#include "saveRestoreTest.h"
#include "engine.h"
#include "deck.h"
#include "card.h"
#include "gameTest.h"
#include "gameCardsTracker.h"

using namespace decore;

SaveRestoreTest::AttackWaitPlayer::AttackWaitPlayer(PlayerSyncData& syncData)
    : mSyncData(syncData)
{

}

const Card& SaveRestoreTest::AttackWaitPlayer::attack(const PlayerId* playerId, const CardSet& cardSet)
{
    (void) playerId;
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

void SaveRestoreTest::PlayerSyncData::signalThread(Engine* engine)
{
    pthread_mutex_lock(&mThreadMutex);
    mEngine = engine;
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

    deck.push_back(Card(SUIT_CLUBS, RANK_6));
    deck.push_back(Card(SUIT_CLUBS, RANK_7));

    engine.setDeck(deck);

    syncData.signalThread(&engine);

    engine.playRound();

    return NULL;
}

void SaveRestoreTest::test00()
{
    pthread_t engineThread;

    PlayerSyncData syncData;

    pthread_create(&engineThread, NULL, attackWaitTestThread, &syncData);

    Engine* engine = syncData.waitForThread();

    CPPUNIT_ASSERT(engine);

    TestWriter savedData;

    engine->save(savedData);

    syncData.signalMove();

    pthread_join(engineThread, NULL);

    CPPUNIT_ASSERT(!savedData.mBytes.empty());
}

void SaveRestoreTest::TestWriter::write(const void* data, unsigned int dataSizeBytes)
{
    const char* dataPtr = static_cast<const char*>(data);
    while (dataSizeBytes--) {
        mBytes.push_back(*dataPtr++);
    }
}