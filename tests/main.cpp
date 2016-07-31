#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cardTest.h"
#include "engineTest.h"

// tests to execute declaration
CPPUNIT_TEST_SUITE_REGISTRATION(CardTest);
CPPUNIT_TEST_SUITE_REGISTRATION(EngineTest);

int main(int, char **)
{

    CppUnit::TextUi::TestRunner runner;

    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

    runner.addTest( registry.makeTest() );

    bool wasSuccessful = runner.run( "", false );

    return !wasSuccessful;
}
