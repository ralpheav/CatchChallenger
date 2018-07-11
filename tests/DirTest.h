#ifndef DIR_TESTCASE_H_INCLUDED
#define DIR_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/util/Dir.h"

class DirTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(DirTestCase);
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
    CatchChallenger::Dir* dir;
    CatchChallenger::Dir* dirpath;
};

#endif //DIR_TESTCASE_H_INCLUDED
