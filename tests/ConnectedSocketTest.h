#if !defined(CONNECTEDSOCKET_TESTCASE_H_INCLUDED)
#define CONNECTEDSOCKET_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/net/ConnectedSocket.h"
#include "general/base/protocol/net/FakeSocket.h"
#include "general/base/protocol/net/HostAddress.h"

class ConnectedSocketTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(ConnectedSocketTestCase);
      CPPUNIT_TEST(abortTest);
      CPPUNIT_TEST(connectToHostTest);
      CPPUNIT_TEST(connectToHost2Test);
      CPPUNIT_TEST(disconnectFromHostTest);
      CPPUNIT_TEST(errorTest);
      CPPUNIT_TEST(flushTest);
      CPPUNIT_TEST(isValidTest);
      CPPUNIT_TEST(setTcpCorkTest);
      CPPUNIT_TEST(localAddressTest);
      CPPUNIT_TEST(localPortTest);
      CPPUNIT_TEST(peerAddressTest);
      CPPUNIT_TEST(peerNameTest);
      CPPUNIT_TEST(peerPortTest);
      CPPUNIT_TEST(getSSLSocket);
      CPPUNIT_TEST(stateTest);
      CPPUNIT_TEST(waitForConnectedTest);
      CPPUNIT_TEST(waitForDisconnectedTest);
      CPPUNIT_TEST(bytesAvailableTest);
      CPPUNIT_TEST(openModeTest);
      CPPUNIT_TEST(errorStringTest);
      CPPUNIT_TEST(readDataTest);
      CPPUNIT_TEST(writeDataTest);
      CPPUNIT_TEST(closeTest);
      CPPUNIT_TEST(isSSLTest);
      CPPUNIT_TEST(isFakeTest);
      CPPUNIT_TEST(isTCPTest);
      CPPUNIT_TEST(existsTest);
      CPPUNIT_TEST(connectTest);
      CPPUNIT_TEST(isSequentialTest);
      CPPUNIT_TEST(canReadLineTest);
      CPPUNIT_TEST(sslErrorsTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void abortTest();
    void connectToHostTest();
    void connectToHost2Test();
    void disconnectFromHostTest();
    void errorTest();
    void flushTest();
    void isValidTest();
    void setTcpCorkTest();
    void localAddressTest();
    void localPortTest();
    void peerAddressTest();
    void peerNameTest();
    void peerPortTest();
    void getSSLSocket();
    void stateTest();
    void waitForConnectedTest();
    void waitForDisconnectedTest();
    void bytesAvailableTest();
    void openModeTest();
    void errorStringTest();
    void readDataTest();
    void writeDataTest();
    void closeTest();
    void isSSLTest();
    void isFakeTest();
    void isTCPTest();
    void existsTest();
    void connectTest();
    void isSequentialTest();
    void canReadLineTest();
    void sslErrorsTest();

private:
    CatchChallenger::FakeSocket* fakeSocket;
    CatchChallenger::ConnectedSocket* connectedSocket;
};

#endif // CONNECTEDSOCKET_TESTCASE_H_INCLUDED
