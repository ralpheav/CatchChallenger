#if !defined(FAKESERVER_TESTCASE_H_INCLUDED)
#define FAKESERVER_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "../general/base/protocol/net/FakeSocket.h"
#include "../general/base/protocol/net/FakeServer.h"

class FakeServerTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(FakeServerTestCase);
      CPPUNIT_TEST(hasPendingConnectionsTest);
      CPPUNIT_TEST(isListeningTest);
      CPPUNIT_TEST(listenTest);
      CPPUNIT_TEST(nextPendingConnectionTest);
      CPPUNIT_TEST(closeTest);
      CPPUNIT_TEST(addPendingConnectionTest);
      CPPUNIT_TEST(disconnectedSocketTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void hasPendingConnectionsTest();
    void isListeningTest();
    void listenTest();
    void nextPendingConnectionTest();
    void closeTest();
    void addPendingConnectionTest();
    void disconnectedSocketTest();

private:
    CatchChallenger::FakeSocket* fakeSocket;
    CatchChallenger::FakeServer* fakeServer;

};

#endif // FAKESERVER_TESTCASE_H_INCLUDED
