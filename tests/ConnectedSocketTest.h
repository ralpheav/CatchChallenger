#if !defined(API_CLIENT_VIRTUAL_TESTCASE_H_INCLUDED)
#define API_CLIENT_VIRTUAL_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/net/ConnectedSocket.h"
#include "general/base/protocol/net/FakeSocket.h"
#include "general/base/protocol/net/HostAddress.h"

class ConnectedSocketTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(ApiClientVirtualTestCase);
      CPPUNIT_TEST(addDataTest);
      CPPUNIT_TEST(getDataTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void abortTest();
    void connectToHostTest();
    void connectToHostTest();
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
    void isSequential();
    void canReadLine();
    void sslErrors();

private:
    CatchChallenger::FakeSocket* fakeSocket;
    CatchChallenger::ConnectedSocket* connectedSocket;
};

#endif // API_CLIENT_VIRTUAL_TESTCASE_H_INCLUDED
