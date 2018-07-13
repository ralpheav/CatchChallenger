#include "Cpp11AdditionTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(Cpp11AdditionTestCase);

void Cpp11AdditionTestCase::setUp()
{
}

void Cpp11AdditionTestCase::tearDown()
{
}

void Cpp11AdditionTestCase::stringreplaceOneTest()
{
    std::string str = "this is a string for testing";
    std::string from = "is";
    std::string to = "for";

    bool result = stringreplaceOne(str, from, to);

    CPPUNIT_ASSERT_EQUAL(result, true);
}

void Cpp11AdditionTestCase::stringreplaceAllTest()
{
    std::string str = "this is a string for testing and this is not the same from above";
    std::string from = "this";
    std::string to = "is";

    bool result = stringreplaceAll(str, from, to);

    CPPUNIT_ASSERT_EQUAL(result, true);
}

void Cpp11AdditionTestCase::stringregexsplitTest()
{
    std::vector<std::string> result;
    const std::string input("some people are on the list");
    const std::regex regex("(\\S+)");

    result = stringregexsplit(input, regex);

    CPPUNIT_ASSERT_EQUAL(result.size(), static_cast<size_t>(6));

    CPPUNIT_ASSERT_EQUAL(result.at(0), std::string("some"));
    CPPUNIT_ASSERT_EQUAL(result.at(1), std::string("people"));
    CPPUNIT_ASSERT_EQUAL(result.at(2), std::string("are"));
    CPPUNIT_ASSERT_EQUAL(result.at(3), std::string("on"));
    CPPUNIT_ASSERT_EQUAL(result.at(4), std::string("the"));
    CPPUNIT_ASSERT_EQUAL(result.at(5), std::string("list"));

}

void Cpp11AdditionTestCase::stringsplitTest()
{
    std::vector<std::string> result;
    const std::string s = "1,4,5,23,2,3,7";
    char delim = ',';

    result = stringsplit(s, delim);

    CPPUNIT_ASSERT_EQUAL(result.size(), static_cast<size_t>(7));

    CPPUNIT_ASSERT_EQUAL(result.at(0), std::string("1"));
    CPPUNIT_ASSERT_EQUAL(result.at(1), std::string("4"));
    CPPUNIT_ASSERT_EQUAL(result.at(2), std::string("5"));
    CPPUNIT_ASSERT_EQUAL(result.at(3), std::string("23"));
    CPPUNIT_ASSERT_EQUAL(result.at(4), std::string("2"));
    CPPUNIT_ASSERT_EQUAL(result.at(5), std::string("3"));
    CPPUNIT_ASSERT_EQUAL(result.at(6), std::string("7"));

}

void Cpp11AdditionTestCase::stringEndsWithTest()
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

void Cpp11AdditionTestCase::stringStartWithTest()
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

void Cpp11AdditionTestCase::stringimplodeTest()
{
    std::vector<std::string> elems{"east","west","north","south"};
    char delim = ',';
    std::string s;
    std::string result = stringimplode(elems, delim, s);
    CPPUNIT_ASSERT_EQUAL(result, std::string("east,west,north,south"));

    std::deque<std::string> deque{"one","two","three"};
    std::queue<std::string> queueElements(deque);
    delim = '+';
    result = stringimplode(queueElements, delim);
    CPPUNIT_ASSERT_EQUAL(result, std::string("one+two+three"));

    std::vector<std::string> elemets{"a","b","c"};
    delim = '-';
    result = stringimplode(elemets, delim);
    CPPUNIT_ASSERT_EQUAL(result, std::string("a-b-c"));

    std::vector<std::string> elemetsNum{"0","1","2"};
    std::string delimt = "+";
    result = stringimplode(elemetsNum, delimt);
    CPPUNIT_ASSERT_EQUAL(result, std::string("0+1+2"));
}

void Cpp11AdditionTestCase::stringtouint8Test()
{
    std::string string = "9";
    bool ok;
    uint8_t res = stringtouint8(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint8_t>(9));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtouint16Test()
{
    std::string string = "12";
    bool ok;
    uint16_t res = stringtouint16(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint16_t>(12));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtouint32Test()
{
    std::string string = "234234";
    bool ok;
    uint32_t res = stringtouint32(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint32_t>(234234));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtoboolTest()
{
    std::string string = "true";
    bool ok;
    bool res = stringtobool(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, true);
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtouint64Test()
{
    std::string string = "3334534333";
    bool ok;
    uint64_t res = stringtouint64(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint64_t>(3334534333));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtoint8Test()
{
    std::string string = "255";
    bool ok;
    int8_t res = stringtoint8(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<int8_t>(255));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtoint16Test()
{
    std::string string = "5558";
    bool ok;
    int8_t res = stringtoint16(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<int8_t>(5558));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtoint32Test()
{
    std::string string = "23423432";
    bool ok;
    int8_t res = stringtoint32(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<int8_t>(23423432));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtoint64Test()
{
    std::string string = "1234244444";
    bool ok;
    int8_t res = stringtoint64(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<int8_t>(1234244444));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtofloatTest()
{
    std::string string = "13.788f";
    bool ok;
    float res = stringtofloat(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<float>(13.788));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::stringtodoubleTest()
{
    std::string string = "22244.112";
    bool ok;
    double res = stringtodouble(string, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<double>(22244.112));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::binarytoHexaTest()
{
    std::vector<char> data = {(char)54, (char)11, (char)95};
    bool ok;
    std::string res = binarytoHexa(data, &ok);
    CPPUNIT_ASSERT_EQUAL(res, std::string("360B5F"));
    CPPUNIT_ASSERT_EQUAL(ok, true);

    const char data1[] = {(char)85, (char)18, (char)65};
    uint32_t vsize = 3;
    res = binarytoHexa(data1, vsize, &ok);
    CPPUNIT_ASSERT_EQUAL(res, std::string("55125F"));
    CPPUNIT_ASSERT_EQUAL(ok, true);

    const unsigned char data2[] = {(unsigned char)85, (unsigned char)18, (unsigned char)65};
    uint32_t size = 3;
    res = binarytoHexa(data2, size, &ok);
    CPPUNIT_ASSERT_EQUAL(res, std::string("55125F"));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::hexToDecUnitTest()
{
    std::string data = "0xF1";
    bool ok;

    uint8_t res = hexToDecUnit(data, &ok);
    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint8_t>(241));
    CPPUNIT_ASSERT_EQUAL(ok, true);
}

void Cpp11AdditionTestCase::hexatoBinaryTest()
{
    std::string data = "0xF1";
    bool ok;

    std::vector<char> res = hexatoBinary(data, &ok);
    CPPUNIT_ASSERT(strcpy(res.data(), "11110001") == 0);
    CPPUNIT_ASSERT_EQUAL(ok, true);

}

void Cpp11AdditionTestCase::binaryAppendTest()
{
    std::vector<char> data{1,7,4,3};
    std::vector<char> add{4,7,0};
    binaryAppend(data, add);
    CPPUNIT_ASSERT_EQUAL(data.at(0), static_cast<char>(1));
    CPPUNIT_ASSERT_EQUAL(data.at(1), static_cast<char>(7));
    CPPUNIT_ASSERT_EQUAL(data.at(2), static_cast<char>(4));
    CPPUNIT_ASSERT_EQUAL(data.at(3), static_cast<char>(3));
    CPPUNIT_ASSERT_EQUAL(data.at(4), static_cast<char>(4));
    CPPUNIT_ASSERT_EQUAL(data.at(5), static_cast<char>(7));
    CPPUNIT_ASSERT_EQUAL(data.at(6), static_cast<char>(0));

    std::vector<char> data1{1, 7, 4, 3};
    const char add1[] = {34, 7, 12};
    const uint32_t addSize = 3;
    binaryAppend(data1, add1, addSize);
    CPPUNIT_ASSERT_EQUAL(data1.at(0), static_cast<char>(1));
    CPPUNIT_ASSERT_EQUAL(data1.at(1), static_cast<char>(7));
    CPPUNIT_ASSERT_EQUAL(data1.at(2), static_cast<char>(4));
    CPPUNIT_ASSERT_EQUAL(data1.at(3), static_cast<char>(3));
    CPPUNIT_ASSERT_EQUAL(data1.at(4), static_cast<char>(34));
    CPPUNIT_ASSERT_EQUAL(data1.at(5), static_cast<char>(7));
    CPPUNIT_ASSERT_EQUAL(data1.at(6), static_cast<char>(12));
}

void Cpp11AdditionTestCase::base64toBinaryTest()
{
    std::string string = "FA10B5";

    std::vector<char> res = base64toBinary(string);
    CPPUNIT_ASSERT(strcpy(res.data(), "111110100001000010110011") == 0);
}

void Cpp11AdditionTestCase::FSabsoluteFilePathTest()
{
    std::string string("/home/yo/Downloads/cmake1.1.0.tar.gz");
    std::string res = FSabsoluteFilePath(string);

    CPPUNIT_ASSERT_EQUAL(res, std::string("cmake1.1.0.tar.gz"));
}

void Cpp11AdditionTestCase::FSabsolutePathTest()
{
    std::string string("/home/yo/Downloads/time.txt");
    std::string res = FSabsolutePath(string);

    CPPUNIT_ASSERT_EQUAL(res, std::string("/home/yo/Downloads/"));
}

void Cpp11AdditionTestCase::msFrom1970Test()
{
    uint64_t res = msFrom1970();

    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint64_t>(1245578405));
}

void Cpp11AdditionTestCase::sFrom1970Test()
{
    uint64_t res = sFrom1970();

    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint64_t>(1245578));
}
