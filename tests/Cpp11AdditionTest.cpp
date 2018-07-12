#include "DataStreamSerializerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(DataStreamSerializerTestCase);

void DataStreamSerializerTestCase::setUp()
{
}

void DataStreamSerializerTestCase::tearDown()
{
}

void DataStreamSerializerTestCase::stringreplaceOneTest()
{
    std::string str = "this is a string for testing";
    std::string from = "is";
    std::string to = "for";

    bool result = stringreplaceOne(str, from, to);

    CPPUNIT_ASSERT_EQUAL(result, true);
}

void DataStreamSerializerTestCase::stringreplaceAllTest()
{
    std::string str = "this is a string for testing and this is not the same from above";
    std::string from = "this";
    std::string to = "is";

    bool result = stringreplaceAll(str, from, to);

    CPPUNIT_ASSERT_EQUAL(result, true);
}

void DataStreamSerializerTestCase::stringregexsplitTest()
{
    std::vector<std::string> result;
    const std::string input("some people are on the list");
    const std::regex regex("(\\S+)");

    result = stringregexsplit(input, regex);

    CPPUNIT_ASSERT_EQUAL(result.size(), 6);

    CPPUNIT_ASSERT_EQUAL(result.at(0), "some");
    CPPUNIT_ASSERT_EQUAL(result.at(1), "people");
    CPPUNIT_ASSERT_EQUAL(result.at(2), "are");
    CPPUNIT_ASSERT_EQUAL(result.at(3), "on");
    CPPUNIT_ASSERT_EQUAL(result.at(4), "the");
    CPPUNIT_ASSERT_EQUAL(result.at(5), "list");

}

void DataStreamSerializerTestCase::stringsplitTest()
{
    std::vector<std::string> result;
    const std::string s = "1,4,5,23,2,3,7";
    char delim = ',';

    result = stringsplit(s, delim);

    CPPUNIT_ASSERT_EQUAL(result.size(), 7);

    CPPUNIT_ASSERT_EQUAL(result.at(0), "1");
    CPPUNIT_ASSERT_EQUAL(result.at(1), "4");
    CPPUNIT_ASSERT_EQUAL(result.at(2), "5");
    CPPUNIT_ASSERT_EQUAL(result.at(3), "23");
    CPPUNIT_ASSERT_EQUAL(result.at(4), "2");
    CPPUNIT_ASSERT_EQUAL(result.at(5), "3");
    CPPUNIT_ASSERT_EQUAL(result.at(6), "7");

}

void DataStreamSerializerTestCase::stringEndsWithTest()
{
    std::string fullString("the christmass is on");
    std::string ending("on");
    bool result = stringEndsWith(fullString, ending);

    CPPUNIT_ASSERT_EQUAL(result, true);

    fullString = "nice to meet the xmas";
    char endingC = 's';
    result = stringEndsWith(fullString, endingC);

    CPPUNIT_ASSERT_EQUAL(result, true);

}

void DataStreamSerializerTestCase::stringStartWithTest()
{
    std::string fullString("the christmass is on");
    std::string starting("the");
    bool result = stringStartWith(fullString, starting);

    CPPUNIT_ASSERT_EQUAL(result, true);

    fullString = "tree slim";
    char startingC = 't';
    result = stringStartWith(fullString, startingC);

    CPPUNIT_ASSERT_EQUAL(result, true);
}

void DataStreamSerializerTestCase::stringimplodeTest()
{
    std::vector<std::string> elems{"east","west","north","south"};
    char delim = ',';
    std::string s;
    std::string result = stringimplode(elems, delim, s);
    CPPUNIT_ASSERT_EQUAL(result, std::string("east,west,north,south"));

    std::queue<std::string> queueElements{"one","two","three"};
    delim = '+';
    result = stringimplode(queueElements, delim);
    CPPUNIT_ASSERT_EQUAL(result, std::string("one+two+three"));

    std::vector<std::string> elemets{"a","b","c"};
    delim = '-';
    result = stringimplode(elemets, delim);
    CPPUNIT_ASSERT_EQUAL(result, std::string("a-b-c"));

    std::vector<std::string> elemetsNum{"0","1","2"};
    std::string delimt = '+';
    std::string result = stringimplode(elemetsNum, delimt);
    CPPUNIT_ASSERT_EQUAL(result, std::string("0+1+2"));
}

void DataStreamSerializerTestCase::stringtouint8Test()
{
    std::string string = "9";
    bool ok;
    uint8_t res = stringtouint8(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 9);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtouint16Test()
{
    std::string string = "12";
    bool ok;
    uint16_t res = stringtouint16(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 12);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtouint32Test()
{
    std::string string = "234234";
    bool ok;
    uint32_t res = stringtouint32(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 234234);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtoboolTest()
{
    std::string string = "true";
    bool ok;
    bool res = stringtobool(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, true);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtouint64Test()
{
    std::string string = "3334534333";
    bool ok;
    uint64_t res = stringtouint64(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 3334534333);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtoint8Test()
{
    std::string string = "255";
    bool ok;
    int8_t res = stringtoint8(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 255);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtoint16Test()
{
    std::string string = "5558";
    bool ok;
    int8_t res = stringtoint16(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 5558);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtoint32Test()
{
    std::string string = "23423432";
    bool ok;
    int8_t res = stringtoint32(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 23423432);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtoint64Test()
{
    std::string string = "1234244444";
    bool ok;
    int8_t res = stringtoint64(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 1234244444);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtofloatTest()
{
    std::string string = "13.788f";
    bool ok;
    float res = stringtofloat(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 13.788);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::stringtodoubleTest()
{
    std::string string = "22244.112";
    bool ok;
    double res = stringtodouble(string, ok);
    CPPUNIT_ASSERT_EQUAL(res, 22244.112);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::binarytoHexaTest()
{
    std::vector<char> data = {(char)54, (char)11, (char)95};
    bool ok;
    std::string res = binarytoHexa(data, ok);
    CPPUNIT_ASSERT_EQUAL(res, std::string("360B5F"));
    CPPUNIT_ASSERT_EQUAL(ok, true);

    const char* data = {(char)85, (char)18, (char)65};
    uint32_t vsize = 3;
    bool* ok;
    res = binarytoHexa(data, vsize, ok);
    CPPUNIT_ASSERT_EQUAL(res, std::string("55125F"));
    CPPUNIT_ASSERT_EQUAL(ok, true);

    const unsigned char* data = {(unsigned char)85, (unsigned char)18, (unsigned char)65};
    uint32_t size = 3;
    bool ok;
    res = binarytoHexa(data, size, ok);
    CPPUNIT_ASSERT_EQUAL(res, std::string("55125F"));
    CPPUNIT_ASSERT_EQUAL(ok, true);

}

void DataStreamSerializerTestCase::hexToDecUnitTest()
{
    std::string data = "0xF1";
    bool ok;

    uint8_t res = hexToDecUnit(data, ok);
    CPPUNIT_ASSERT_EQUAL(res, 241);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void DataStreamSerializerTestCase::hexatoBinaryTest()
{
    std::string data = "0xF1";
    bool ok;

    std::vector<char> res = hexatoBinary(data, ok);
    CPPUNIT_ASSERT(strcpy(res.data(), "11110001") == 0);
    CPPUNIT_ASSERT_EQUAL(ok, true);

}

void DataStreamSerializerTestCase::binaryAppendTest()
{
    std::vector<char> data{1,7,4,3};
    std::vector<char> add{4,7,0};
    binaryAppend(data, add);
    CPPUNIT_ASSERT_EQUAL(data.at(0), 1);
    CPPUNIT_ASSERT_EQUAL(data.at(1), 7);
    CPPUNIT_ASSERT_EQUAL(data.at(2), 4);
    CPPUNIT_ASSERT_EQUAL(data.at(3), 3);
    CPPUNIT_ASSERT_EQUAL(data.at(4), 4);
    CPPUNIT_ASSERT_EQUAL(data.at(5), 7);
    CPPUNIT_ASSERT_EQUAL(data.at(6), 0);

    std::vector<char> data1{1,7,4,3};
    const char* const add1 = {34,7,12};
    const uint32_t addSize = 3;
    binaryAppend(data1, add1, addSize);
    CPPUNIT_ASSERT_EQUAL(data1.at(0), 1);
    CPPUNIT_ASSERT_EQUAL(data1.at(1), 7);
    CPPUNIT_ASSERT_EQUAL(data1.at(2), 4);
    CPPUNIT_ASSERT_EQUAL(data1.at(3), 3);
    CPPUNIT_ASSERT_EQUAL(data1.at(4), 34);
    CPPUNIT_ASSERT_EQUAL(data1.at(5), 7);
    CPPUNIT_ASSERT_EQUAL(data1.at(6), 12);
}

void DataStreamSerializerTestCase::base64toBinaryTest()
{
    std::string string = "FA10B5";

    std::vector<char> res = base64toBinary(string);
    CPPUNIT_ASSERT(strcpy(res.data(), "111110100001000010110011") == 0);
}

void DataStreamSerializerTestCase::FSabsoluteFilePathTest()
{
    std::string string("/home/yo/Downloads/cmake1.1.0.tar.gz");
    std::string res = FSabsoluteFilePath(string);

    CPPUNIT_ASSERT_EQUAL(res, std::string("cmake1.1.0.tar.gz"));
}

void DataStreamSerializerTestCase::FSabsolutePathTest()
{
    std::string string("/home/yo/Downloads/time.txt");
    std::string res = FSabsolutePath(string);

    CPPUNIT_ASSERT_EQUAL(res, std::string("/home/yo/Downloads/"));
}

void DataStreamSerializerTestCase::msFrom1970Test()
{
    uint64_t res = msFrom1970();

    CPPUNIT_ASSERT_EQUAL(res, 1245578405);
}

void DataStreamSerializerTestCase::sFrom1970Test()
{
    uint64_t res = sFrom1970();

    CPPUNIT_ASSERT_EQUAL(res, 1245578);
}
