#ifndef SAVERESTORETEST_H
#define SAVERESTORETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <pthread.h>
#include <vector>

#include "player.h"
#include "engine.h"
#include "basePlayer.h"
#include "dataWriter.h"
#include "dataReader.h"

using namespace decore;

class SaveRestoreTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SaveRestoreTest);
    CPPUNIT_TEST(test00);
    CPPUNIT_TEST_SUITE_END();

public:
    void test00();

private:

    static const unsigned int MAX_CARDS;

    class PlayerSyncData
    {
        pthread_mutex_t mMoveMutex;
        pthread_cond_t mMoveSignal;
        bool mDontWaitForMove;

        pthread_mutex_t mThreadMutex;
        pthread_cond_t mThreadSignal;
        Engine* mEngine;

    public:

        PlayerSyncData();
        ~PlayerSyncData();

        void signalMove();
        void waitForMove();

        void setEngine(Engine* engine);
        void signalThread();
        Engine* waitForThread();
    };

    class ThreadData
    {
    public:
        Player& mPlayer0;
        Player& mPlayer1;
        PlayerSyncData& mSyncData;
        ThreadData(Player& player0, Player& player1, PlayerSyncData& syncData);
    };

    class AttackWaitPlayer : public BasePlayer
    {
        PlayerSyncData& mSyncData;
        unsigned int mMoveCount;
    public:
        AttackWaitPlayer(PlayerSyncData& syncData, unsigned int moveCount);
        const Card& attack(const PlayerId* playerId, const CardSet& cardSet);
        const Card* pitch(const PlayerId* playerId, const CardSet& cardSet);
    private:
        void processMove();
    };

    class DefendWaitPlayer : public BasePlayer
    {
        PlayerSyncData& mSyncData;
    public:
        DefendWaitPlayer(PlayerSyncData& syncData);
        const Card* defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet);
    };

    class TestWriter : public DataWriter
    {
    public:
        std::vector<unsigned char> mBytes;
    protected:
        void write(const void* data, unsigned int dataSizeBytes);
        unsigned int position() const;
    };

    class TestReader : public DataReader
    {
        unsigned int mByteIndex;
    public:
        const std::vector<unsigned char>& mBytes;
        TestReader(const std::vector<unsigned char>& bytes);

    protected:
        void read(void* data, unsigned int dataSizeBytes);
        unsigned int position() const;
    };
    static void* attackWaitTestThread(void* data);
    static void generate(Deck& deck);
};

#endif /* SAVERESTORETEST_H */

