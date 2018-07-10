#if !defined(DATASTREAM_TESTCASE_H_INCLUDED)
#define DATASTREAM_TESTCASE_H_INCLUDED

#include <string>
#include <vector>

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/util/ByteArray.h"

class ByteArrayTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(ByteArrayTestCase);
      CPPUNIT_TEST(loadTest);
      CPPUNIT_TEST(storeTest);
      CPPUNIT_TEST(getDataTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void loadTest();
    void storeTest();
    void getDataTest();

private:
    ByteArray *bytearray;
    ByteArray *Bytearrayb8;
    ByteArray *bytearrayCs; 
};

#endif
