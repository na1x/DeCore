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
    CPPUNIT_TEST_SUITE_END();

public:

    void testCreate();
    void testGenerate();
    void testAdd();
    void testShuffle();
    void testGet();
};

#endif // CARDUTILSTEST_H
