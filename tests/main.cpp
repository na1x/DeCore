#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cardTest.h"
#include "rulesTest.h"
#include "engineTest.h"
#include "gameTest.h"
#include "saveRestoreTest.h"

// tests to execute declaration
CPPUNIT_TEST_SUITE_REGISTRATION(CardTest);
CPPUNIT_TEST_SUITE_REGISTRATION(RulesTest);
CPPUNIT_TEST_SUITE_REGISTRATION(EngineTest);
CPPUNIT_TEST_SUITE_REGISTRATION(GameTest);
CPPUNIT_TEST_SUITE_REGISTRATION(SaveRestoreTest);

int main(int, char **)
{

    CppUnit::TextUi::TestRunner runner;

    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

    runner.addTest( registry.makeTest() );

    bool wasSuccessful = runner.run( "", false );

    return !wasSuccessful;
}
