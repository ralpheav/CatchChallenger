#if !defined(FILE_TESTCASE_H_INCLUDED)
#define FILE_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/util/File.h"

class FileTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(FileTestCase);
      CPPUNIT_TEST(loadTest);
      CPPUNIT_TEST(storeTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void loadTest();
    void storeTest();

private:
    File* file;
};

#endif
