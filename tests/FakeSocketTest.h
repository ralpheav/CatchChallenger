#if !defined(FAKESOCKET_TESTCASE_H_INCLUDED)
#define FAKESOCKET_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/net/FakeSocket.h"

class FakeSocketTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(FakeSocketTestCase);
      CPPUNIT_TEST(openTest);
      CPPUNIT_TEST(abortTest);
      CPPUNIT_TEST(disconnectFromHostTest);
      CPPUNIT_TEST(disconnectFromFakeServerTest);
      CPPUNIT_TEST(connectToHostTest);
      CPPUNIT_TEST(bytesAvailableTest);
      CPPUNIT_TEST(closeTest);
      CPPUNIT_TEST(getTheOtherSocketTest);
      CPPUNIT_TEST(getRXSizeTest);
      CPPUNIT_TEST(getTXSizeTest);
      CPPUNIT_TEST(stateTest);
      CPPUNIT_TEST(isValidTest);
      CPPUNIT_TEST(socketDescriptorTest);
      CPPUNIT_TEST(flushTest);
      CPPUNIT_TEST(localAddressTest);
      CPPUNIT_TEST(localPortTest);
      CPPUNIT_TEST(peerAddressTest);
      CPPUNIT_TEST(peerNameTest);
      CPPUNIT_TEST(peerPortTest);
      CPPUNIT_TEST(errorStringTest);
      CPPUNIT_TEST(openModeTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void openTest();
    void abortTest();
    void disconnectFromHostTest();
    void disconnectFromFakeServerTest();
    void connectToHostTest();
    void bytesAvailableTest();
    void closeTest();
    void getTheOtherSocketTest();
    void getRXSizeTest();
    void getTXSizeTest();
    void stateTest();
    void isValidTest();
    void socketDescriptorTest();
    void flushTest();
    void localAddressTest();
    void localPortTest();
    void peerAddressTest();
    void peerNameTest();
    void peerPortTest();
    void errorStringTest();
    void openModeTest();

private:
    CatchChallenger::FakeSocket* fakeSocket;

};

#endif // FAKESOCKET_TESTCASE_H_INCLUDED
