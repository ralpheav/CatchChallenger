#if !defined(CPP11ADDITION_TESTCASE_H_INCLUDED)
#define CPP11ADDITION_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/util/Cpp11Addition.h"

class Cpp11AdditionTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(DataStreamSerializerTestCase);
      CPPUNIT_TEST(stringreplaceOneTest);
      CPPUNIT_TEST(stringreplaceAllTest);
      CPPUNIT_TEST(stringregexsplitTest);
      CPPUNIT_TEST(stringsplitTest);
      CPPUNIT_TEST(stringEndsWithTest);
      CPPUNIT_TEST(stringStartWithTest);
      CPPUNIT_TEST(stringimplodeTest);
      CPPUNIT_TEST(stringtouint8Test);
      CPPUNIT_TEST(stringtouint16Test);
      CPPUNIT_TEST(stringtouint32Test);
      CPPUNIT_TEST(stringtoboolTest);
      CPPUNIT_TEST(stringtouint64Test);
      CPPUNIT_TEST(stringtoint8Test);
      CPPUNIT_TEST(stringtoint16Test);
      CPPUNIT_TEST(stringtoint32Test);
      CPPUNIT_TEST(stringtoint64Test);
      CPPUNIT_TEST(stringtofloatTest);
      CPPUNIT_TEST(stringtodoubleTest);
      CPPUNIT_TEST(binarytoHexaTest);
      CPPUNIT_TEST(hexToDecUnitTest);
      CPPUNIT_TEST(hexatoBinaryTest);
      CPPUNIT_TEST(binaryAppendTest);
      CPPUNIT_TEST(base64toBinaryTest);
      CPPUNIT_TEST(FSabsoluteFilePathTest);
      CPPUNIT_TEST(FSabsolutePathTest);
      CPPUNIT_TEST(msFrom1970Test);
      CPPUNIT_TEST(sFrom1970Test);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:

    void stringreplaceOneTest();
    void stringreplaceAllTest();
    void stringregexsplitTest();
    void stringsplitTest();
    void stringEndsWithTest();
    void stringStartWithTest();
    void stringimplodeTest();
    void stringtouint8Test();
    void stringtouint16Test();
    void stringtouint32Test();
    void stringtoboolTest();
    void stringtouint64Test();
    void stringtoint8Test();
    void stringtoint16Test();
    void stringtoint32Test();
    void stringtoint64Test();
    void stringtofloatTest();
    void stringtodoubleTest();
    void binarytoHexaTest();
    void hexToDecUnitTest();
    void hexatoBinaryTest();
    void binaryAppendTest();
    void base64toBinaryTest();
    void FSabsoluteFilePathTest();
    void FSabsolutePathTest();
    void msFrom1970Test();
    void sFrom1970Test();

};

#endif //CPP11ADDITION_TESTCASE_H_INCLUDED
