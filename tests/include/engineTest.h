#ifndef ENGINETEST_H
#define ENGINETEST_H
#include <cppunit/extensions/HelperMacros.h>
#include "player.h"
#include "basePlayer.h"

class EngineTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(EngineTest);
    CPPUNIT_TEST(testAddPlayers);
    CPPUNIT_TEST(testAddDuplicatedPlayers);
    CPPUNIT_TEST_SUITE_END();

public:
    void testAddPlayers();
    void testAddDuplicatedPlayers();

private:
    class TestPlayer : public BasePlayer
    {
    public:
        std::vector<const decore::PlayerId*> mIds;
        void idCreated(const decore::PlayerId *id);
    };

};

#endif // ENGINETEST_H
