#if !defined(HOSTADDRESS_TESTCASE_H_INCLUDED)
#define HOSTADDRESS_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/net/HostAddress.h"

class HostAddressTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(HostAddressTestCase);
      CPPUNIT_TEST(setInstanceTest);
      CPPUNIT_TEST(setAddressTest);
      CPPUNIT_TEST(clearTest);
      CPPUNIT_TEST(isEqualTest);
      CPPUNIT_TEST(isMulticastTest);
      CPPUNIT_TEST(isLoopbackTest);
      CPPUNIT_TEST(toStringTest);
      CPPUNIT_TEST(hexStrTest);
      CPPUNIT_TEST(toIpv6Test);
      CPPUNIT_TEST(setWithSpecialAddressTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void setInstanceTest();
    void setAddressTest();
    void clearTest();
    void isEqualTest();
    void isMulticastTest();
    void isLoopbackTest();
    void toStringTest();
    void hexStrTest();
    void toIpv6Test();
    void setWithSpecialAddressTest();

private:
    CatchChallenger::HostAddress* hostaddress;

};

#endif // HOSTADDRESS_TESTCASE_H_INCLUDED
