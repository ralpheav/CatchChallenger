#include "ByteArrayTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ByteArrayTestCase);


void ByteArrayTestCase::setUp()
{
    bytearray = new ByteArray();

    unsigned char usource[] = {0x0f, 0x01, 0x0b, 0x04, 0x00};

    bytearrayb8 = new ByteArray(usource, static_cast<size_t>(4));

    const char csource[] = {(char)0xa1, (char)0x17, (char)0x0d, (char)0xe0, (char)0x00};

    bytearrayCs = new ByteArray(csource, static_cast<size_t>(4));
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

    CPPUNIT_ASSERT_EQUAL(bytearray->size(), static_cast<size_t>(4));

    CPPUNIT_ASSERT_EQUAL(bytearrayb8->size(), static_cast<size_t>(4));
    
    CPPUNIT_ASSERT_EQUAL(bytearrayCs->size(), static_cast<size_t>(4));

}


void ByteArrayTestCase::storeTest()
{
    std::vector<uint8_t> list = bytearray->get();

    CPPUNIT_ASSERT(list.at(0) == 1);
    CPPUNIT_ASSERT(list.at(1) == 2);
    CPPUNIT_ASSERT(list.at(2) == 3);
    CPPUNIT_ASSERT(list.at(3) == 4);
    CPPUNIT_ASSERT(list.size() == 4);

    list.clear();

    list = bytearrayb8->get();

    CPPUNIT_ASSERT(list.at(0) == 0x0f);
    CPPUNIT_ASSERT(list.at(1) == 0x01);
    CPPUNIT_ASSERT(list.at(2) == 0x0b);
    CPPUNIT_ASSERT(list.at(3) == 0x04);
    CPPUNIT_ASSERT(list.size() == 4);

    list.clear();

    list = bytearrayCs->get();

    CPPUNIT_ASSERT(list.at(0) == (char)0xa1);
    CPPUNIT_ASSERT(list.at(1) == (char)0x17);
    CPPUNIT_ASSERT(list.at(2) == (char)0x0d);
    CPPUNIT_ASSERT(list.at(3) == (char)0xe0);
    CPPUNIT_ASSERT(list.size() == 4);

    list.clear();



    std::vector<uint8_t>* ptrlist = bytearray->ptrget();

    CPPUNIT_ASSERT(ptrlist->at(0) == 1);
    CPPUNIT_ASSERT(ptrlist->at(1) == 2);
    CPPUNIT_ASSERT(ptrlist->at(2) == 3);
    CPPUNIT_ASSERT(ptrlist->at(3) == 4);
    CPPUNIT_ASSERT(ptrlist->size() == 4);

    ptrlist->clear();

    ptrlist = bytearrayb8->ptrget();

    CPPUNIT_ASSERT(ptrlist->at(0) == 0x0f);
    CPPUNIT_ASSERT(ptrlist->at(1) == 0x01);
    CPPUNIT_ASSERT(ptrlist->at(2) == 0x0b);
    CPPUNIT_ASSERT(ptrlist->at(3) == 0x04);
    CPPUNIT_ASSERT(ptrlist->size() == 4);

    ptrlist->clear();

    ptrlist = bytearrayCs->ptrget();

    CPPUNIT_ASSERT(ptrlist->at(0) == (char)0xa1);
    CPPUNIT_ASSERT(ptrlist->at(1) == (char)0x17);
    CPPUNIT_ASSERT(ptrlist->at(2) == (char)0x0d);
    CPPUNIT_ASSERT(ptrlist->at(3) == (char)0xe0);
    CPPUNIT_ASSERT(ptrlist->size() == 4);

    ptrlist->clear();



    std::string hexa = bytearray->toHex();

    //CPPUNIT_ASSERT(hexa == "");//TODO

    hexa = bytearrayb8->toHex();

    //CPPUNIT_ASSERT(hexa == "");//TODO

    hexa = bytearrayCs->toHex();

    CPPUNIT_ASSERT(hexa == "");//TODO

}

void ByteArrayTestCase::getDataTest() {

    std::string data = bytearray->constData();

    //CPPUNIT_ASSERT(data == "");//TODO

    data = bytearrayb8->constData();

    //CPPUNIT_ASSERT(data == "");//TODO

    data = bytearrayCs->constData();

    //CPPUNIT_ASSERT(data == "");//TODO

    
    
    data = bytearray->data();

    //CPPUNIT_ASSERT(data == "");//TODO

    data = bytearrayb8->data();

    //CPPUNIT_ASSERT(data == "");//TODO

    data = bytearrayCs->data();

    //CPPUNIT_ASSERT(data == "");//TODO

}
