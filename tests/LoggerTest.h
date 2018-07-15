#if !defined(LOGGER_TESTCASE_H_INCLUDED)
#define LOGGER_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/log/logger.h"
#include <fstream>
#include <cstring>

class LoggerTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(LoggerTestCase);
      CPPUNIT_TEST(LoggerInstanceTest);
      CPPUNIT_TEST(logTest);
      CPPUNIT_TEST(logHelperTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void LoggerInstanceTest();
    void logTest();
    void logHelperTest();

private:
    //no vars

};

#endif // LOGGER_TESTCASE_H_INCLUDED
