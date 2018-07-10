#if !defined(DATASTREAMSERIALIZER_TESTCASE_H_INCLUDED)
#define DATASTREAMSERIALIZER_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/util/DataStreamSerializer.h"

class DataStreamSerializerTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(DataStreamSerializerTestCase);
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
    DataStreamSerializer* datastream;
};

#endif
