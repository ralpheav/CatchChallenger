#include "ByteArrayTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ByteArrayTestCase);


void ByteArrayTestCase::setUp()
{
    bytearray = new ByteArray();

    bytearrayb8 = new ByteArray({0x01,0x02,0x03,0x04}, 4);

    bytearrayCs = new ByteArray({'a','b','c','d'}, 4);
}

void ByteArrayTestCase::tearDown()
{
    delete bytearray;
    delete bytearrayb8;
    delete bytearrayCs;

    bytearray = NULL;
    bytearrayb8 = NULL;
    bytearrayCs = NULL;
}

void ByteArrayTestCase::loadTest()
{
    // our load test logic
    bytearray->push_back(1);
    bytearray->push_back(2);
    bytearray->push_back(3);
    bytearray->push_back(4);

    CPPUNIT_ASSERT_EQUAL(bytearray->size(), 4);

    CPPUNIT_ASSERT_EQUAL(bytearrayb8->size(), 4);
    
    CPPUNIT_ASSERT_EQUAL(bytearrayCs->size(), 4);

}


void DiskDataTestCase::storeTest()
{
    std::vector<uint8_t> list = bytearray->get();

    CPPUNIT_ASSERT(list == "");//TODO

    list->clean();

    list = bytearrayb8->get();

    CPPUNIT_ASSERT(list == "");//TODO

    list->clean();

    list = bytearrayCs->get();

    CPPUNIT_ASSERT(list == "");//TODO

    list->clean();



    std::vector<uint8_t>* ptrlist = bytearray->ptrget();

    CPPUNIT_ASSERT(ptrlist == "");//TODO

    ptrlist->clean();

    ptrlist = bytearrayb8->get();

    CPPUNIT_ASSERT(ptrlist == "");//TODO

    ptrlist->clean();

    ptrlist = bytearrayCs->get();

    CPPUNIT_ASSERT(ptrlist == "");//TODO

    ptrlist->clean();



    std::string hexa = bytearray->toHex();

    CPPUNIT_ASSERT(hexa == "");//TODO

    hexa = bytearrayb8->ptrget();

    CPPUNIT_ASSERT(hexa == "");//TODO

    hexa = bytearrayCs->ptrget();

    CPPUNIT_ASSERT(hexa == "");//TODO

}

void ByteArrayTestCase::getDataTest() {

    std::string data = bytearray->constData();

    CPPUNIT_ASSERT(data == "");//TODO

    data = bytearrayb8->constData();

    CPPUNIT_ASSERT(data == "");//TODO

    data = bytearrayCs->constData();

    CPPUNIT_ASSERT(data == "");//TODO

    
    
    data = bytearray->data();

    CPPUNIT_ASSERT(data == "");//TODO

    data = bytearrayb8->data();

    CPPUNIT_ASSERT(data == "");//TODO

    data = bytearrayCs->data();

    CPPUNIT_ASSERT(data == "");//TODO

}
