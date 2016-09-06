#ifndef RULESTEST_H
#define RULESTEST_H

#include <cppunit/extensions/HelperMacros.h>

class RulesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(RulesTest);
    CPPUNIT_TEST(testPickNext00);
    CPPUNIT_TEST(testPickNext01);
    CPPUNIT_TEST(testAttackCards);
    CPPUNIT_TEST(testDefendCards);
    CPPUNIT_TEST(testDeal0);
    CPPUNIT_TEST(testDeal1);
    CPPUNIT_TEST_SUITE_END();

public:
    void testPickNext00();
    void testPickNext01();
    void testAttackCards();
    void testDefendCards();
    void testDeal0();
    void testDeal1();
};


#endif // RULESTEST_H
