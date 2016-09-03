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
#include "gameCardsTracker.h"

using namespace decore;

class SaveRestoreTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SaveRestoreTest);
    CPPUNIT_TEST(testDataReader);
    CPPUNIT_TEST(test00);
    CPPUNIT_TEST(test01);
    CPPUNIT_TEST(test02);
    CPPUNIT_TEST(test03);
    CPPUNIT_TEST(test04);
    CPPUNIT_TEST_SUITE_END();

public:
    void testDataReader();
    void test00();
    void test01();
    void test02();
    void test03();
    void test04();

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

    class WaitPlayer : public BasePlayer
    {
        PlayerSyncData& mSyncData;
        unsigned int mMoveCount;

    protected:
        WaitPlayer(PlayerSyncData& syncData, unsigned int moveCount);
        void processMove();
    };

    class AttackWaitPlayer : public WaitPlayer
    {
    public:
        AttackWaitPlayer(PlayerSyncData& syncData, unsigned int moveCount);
        const Card& attack(const PlayerId* playerId, const CardSet& cardSet);
        const Card* pitch(const PlayerId* playerId, const CardSet& cardSet);
    };

    class DefendWaitPlayer : public WaitPlayer
    {
    public:
        DefendWaitPlayer(PlayerSyncData& syncData, unsigned int moveCount);
        const Card* defend(const PlayerId* playerId, const Card& attackCard, const CardSet& cardSet);
    };

    class TestWriter : public DataWriter
    {
    public:
        using DataWriter::write;
        std::vector<unsigned char> mBytes;
    protected:
        void write(const void* data, unsigned int dataSizeBytes);
        unsigned int position() const;
    };

    class TestReader : public DataReader
    {
    public:
        unsigned int mByteIndex;
        using DataReader::read;
        const std::vector<unsigned char>& mBytes;
        TestReader(const std::vector<unsigned char>& bytes);

    protected:
        void read(void* data, unsigned int dataSizeBytes);
        unsigned int position() const;
    };
    static void* testThread(void* data);
    static void generate(Deck& deck);
    static void test(Player& player0, Player& player1, Player& restoredPlayer0, Player& restoredPlayer1, PlayerSyncData& syncData, GameCardsTracker& restoredTracker, Engine& restored);
    static void checkNoDeal(std::vector<BasePlayer*> players);
};

#endif /* SAVERESTORETEST_H */

