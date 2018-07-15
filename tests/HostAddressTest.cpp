#include "HostAddressTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(HostAddressTestCase);

using namespace CatchChallenger;

void HostAddressTestCase::setUp()
{
    hostaddress = new HostAddress();
}

void HostAddressTestCase::tearDown()
{
    delete hostaddress;
    hostaddress = NULL;
}

void HostAddressTestCase::setInstanceTest()
{
    hostaddress = new HostAddress(std::string("123,45,12,1"));

    CPPUNIT_ASSERT(hostaddress != nullptr);
    CPPUNIT_ASSERT_EQUAL(hostaddress->toString(), std::string("123,45,12,1"));

    hostaddress = new HostAddress(HostAddress::Null);

    CPPUNIT_ASSERT(hostaddress != nullptr);
    CPPUNIT_ASSERT_EQUAL(hostaddress->toString(), std::string("0.0.0.0"));

    hostaddress = new HostAddress();

    CPPUNIT_ASSERT(hostaddress != nullptr);
}

void HostAddressTestCase::setAddressTest()
{
    hostaddress->setAddress(HostAddress::LocalHost);

    CPPUNIT_ASSERT_EQUAL(hostaddress->toString(), std::string("127.0.0.1"));

    hostaddress->setAddress(std::string("125.125.22.10"));

    CPPUNIT_ASSERT_EQUAL(hostaddress->toString(), std::string("125.125.22.10"));
}

void HostAddressTestCase::clearTest() {
    hostaddress->clear();

    CPPUNIT_ASSERT_EQUAL(hostaddress->toString().empty(), true);
}

void HostAddressTestCase::isEqualTest() {
    HostAddress hosta(HostAddress::Null);
    hostaddress->setAddress(HostAddress::LocalHost);
    bool res = hostaddress->isEqual(hosta);

    CPPUNIT_ASSERT_EQUAL(res, false);

    hosta.setAddress(HostAddress::LocalHost);
    res = hostaddress->isEqual(hosta);

    CPPUNIT_ASSERT_EQUAL(res, true);

}

void HostAddressTestCase::isMulticastTest() {
    bool res = hostaddress->isMulticast();

    CPPUNIT_ASSERT_EQUAL(res, false);
}

void HostAddressTestCase::isLoopbackTest() {
    bool res = hostaddress->isLoopback();

    CPPUNIT_ASSERT_EQUAL(res, false);
}

void HostAddressTestCase::toStringTest() {
    std::string host = hostaddress->toString();

    CPPUNIT_ASSERT_EQUAL(host, std::string("127.0.0.1"));
}

void HostAddressTestCase::hexStrTest() {
    unsigned char dataIp[] = {0x12, 0x32, 0x41, 0x01};
    std::string ip = hostaddress->hexStr(dataIp, 0, 4);

    CPPUNIT_ASSERT_EQUAL(ip, std::string("12.50.65.1"));
}

void HostAddressTestCase::toIpv6Test() {
    std::string ip = hostaddress->toIpv6();

    CPPUNIT_ASSERT_EQUAL(ip, std::string("42:5a:12:14:b0:01"));
}

void HostAddressTestCase::setWithSpecialAddressTest() {
     HostAddress hostaddress2 = HostAddress::Null;

    CPPUNIT_ASSERT_EQUAL(hostaddress2.toString(), std::string("0.0.0.0"));
}
