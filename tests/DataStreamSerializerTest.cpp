#include "DataStreamSerializerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(DataStreamSerializerTestCase);

using namespace CatchChallenger;

void DataStreamSerializerTestCase::setUp()
{
    datastream = new DataStreamSerializer(static_cast<const unsigned int>(8));
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
    uint8_t byte;

    *datastream << 0x01;
    *datastream << 0x02;
    *datastream << 0x03;
    *datastream << 0x04;
    *datastream << 0x05;
    *datastream << 0x06;
    *datastream << 0x07;
    *datastream << 0x08;

    *datastream >> byte;
    CPPUNIT_ASSERT_EQUAL(byte, static_cast<uint8_t>(0x01));

    *datastream >> byte;
    CPPUNIT_ASSERT_EQUAL(byte, static_cast<uint8_t>(0x02));

    *datastream >> byte;
    CPPUNIT_ASSERT_EQUAL(byte, static_cast<uint8_t>(0x03));

    *datastream >> byte;
    CPPUNIT_ASSERT_EQUAL(byte, static_cast<uint8_t>(0x04));

    *datastream >> byte;
    CPPUNIT_ASSERT_EQUAL(byte, static_cast<uint8_t>(0x05));

    *datastream >> byte;
    CPPUNIT_ASSERT_EQUAL(byte, static_cast<uint8_t>(0x06));

    *datastream >> byte;
    CPPUNIT_ASSERT_EQUAL(byte, static_cast<uint8_t>(0x07));

    *datastream >> byte;
    CPPUNIT_ASSERT_EQUAL(byte, static_cast<uint8_t>(0x08));

}
