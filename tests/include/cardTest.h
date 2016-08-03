#ifndef CARDUTILSTEST_H
#define CARDUTILSTEST_H

#include <cppunit/extensions/HelperMacros.h>

class CardTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CardTest);
    CPPUNIT_TEST(testCreate);
    CPPUNIT_TEST(testGenerate);
    CPPUNIT_TEST(testAdd);
    CPPUNIT_TEST(testShuffle);
    CPPUNIT_TEST(testGet);
    CPPUNIT_TEST(testGetByRank);
    CPPUNIT_TEST(testGetBySuit);
    CPPUNIT_TEST(testIntersection);
    CPPUNIT_TEST_SUITE_END();

public:

    void testCreate();
    void testGenerate();
    void testAdd();
    void testShuffle();
    void testGet();
    void testGetByRank();
    void testGetBySuit();
    void testIntersection();
};

#endif // CARDUTILSTEST_H
