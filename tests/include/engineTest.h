#ifndef ENGINETEST_H
#define ENGINETEST_H
#include <cppunit/extensions/HelperMacros.h>

class EngineTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(EngineTest);
    CPPUNIT_TEST(testPickNext);
    CPPUNIT_TEST_SUITE_END();

public:
    void testPickNext();

};

#endif // ENGINETEST_H
