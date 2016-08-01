#ifndef ENGINETEST_H
#define ENGINETEST_H
#include <cppunit/extensions/HelperMacros.h>

class EngineTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(EngineTest);
    CPPUNIT_TEST(testAddPlayers);
    CPPUNIT_TEST(testAddDuplicatedPlayers);
    CPPUNIT_TEST(testPickNext);
    CPPUNIT_TEST_SUITE_END();

public:
    void testAddPlayers();
    void testAddDuplicatedPlayers();
    void testPickNext();

};

#endif // ENGINETEST_H
