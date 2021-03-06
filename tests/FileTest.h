#if !defined(FILE_TESTCASE_H_INCLUDED)
#define FILE_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/util/File.h"

class FileTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(FileTestCase);
      CPPUNIT_TEST(loadTest);
      CPPUNIT_TEST(writeTest);
      CPPUNIT_TEST(readTest);
      CPPUNIT_TEST(removeTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void loadTest();
    void writeTest();
    void readTest();
    void removeTest();


private:
    CatchChallenger::File* file;
    CatchChallenger::File* file1;
    CatchChallenger::File* file2;
    CatchChallenger::File* file3;
    CatchChallenger::File* file4;

    struct Config {
        unsigned int id;
        char type[8];
        int flag;
    };
};

#endif
