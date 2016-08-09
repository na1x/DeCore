#ifndef GAMETEST_H
#define GAMETEST_H
#include <cppunit/extensions/HelperMacros.h>

class GameTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GameTest);
    CPPUNIT_TEST(testInitialization);
    CPPUNIT_TEST(testOneRound);
    CPPUNIT_TEST_SUITE_END();

public:
    void testInitialization();
    void testOneRound();

};

#endif // GAMETEST_H
