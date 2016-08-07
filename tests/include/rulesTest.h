#ifndef RULESTEST_H
#define RULESTEST_H

#include <cppunit/extensions/HelperMacros.h>

class RulesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(RulesTest);
    CPPUNIT_TEST(testPickNext);
    CPPUNIT_TEST(testAttackCards);
    CPPUNIT_TEST(testDefendCards);
    CPPUNIT_TEST_SUITE_END();

public:
    void testPickNext();
    void testAttackCards();
    void testDefendCards();
};


#endif // RULESTEST_H
