#include "DataStreamSerializerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(DataStreamSerializerTestCase);

void DataStreamSerializerTestCase::setUp()
{
    datastream = new DataStreamSerializer(8);
}

void DataStreamSerializerTestCase::tearDown()
{
    delete datastream;
    datastream = NULL;
}


void DataStreamSerializerTestCase::loadTest()
{
    // our load test logic
    bool result = datastream->isOpen();

    CPPUNIT_ASSERT_EQUAL(result, false);
}


void DataStreamSerializerTestCase::storeTest()
{
    std::string data = "24354435";
    std::string out;
    datastream->write(data);
    datastream->read(out);

    CPPUNIT_ASSERT_EQUAL(data, out);
}
