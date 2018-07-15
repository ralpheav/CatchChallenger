#include "FakeServerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FakeServerTestCase);

using namespace CatchChallenger;

void FakeServerTestCase::setUp()
{
    fakeSocket = new FakeSocket();
    fakeServer = &FakeServer::server;
}

void FakeServerTestCase::tearDown()
{
    delete fakeSocket;
    delete fakeServer;

    fakeSocket = nullptr;
    fakeServer = nullptr;
}

void FakeServerTestCase::hasPendingConnectionsTest()  {
    bool res = fakeServer->hasPendingConnections();

    CPPUNIT_ASSERT_EQUAL(res, true);
}

void FakeServerTestCase::isListeningTest()  {
    bool res = fakeServer->isListening();

    CPPUNIT_ASSERT_EQUAL(res, true);
}

void FakeServerTestCase::listenTest()  {
    bool res = fakeServer->listen();

    CPPUNIT_ASSERT_EQUAL(res, false);
}

void FakeServerTestCase::nextPendingConnectionTest()  {
    FakeSocket* fake = new FakeSocket();
    fakeServer->addPendingConnection(fake);
    FakeSocket* next = fakeServer->nextPendingConnection();

    CPPUNIT_ASSERT(next != nullptr);
    CPPUNIT_ASSERT(next == fake);
}

void FakeServerTestCase::closeTest()  {
    fakeServer->close();

    CPPUNIT_ASSERT_EQUAL(true, true);
}

void FakeServerTestCase::addPendingConnectionTest()  {
    fakeServer->addPendingConnection(fakeSocket);

    FakeSocket* next = fakeServer->nextPendingConnection();

    CPPUNIT_ASSERT_EQUAL(fakeSocket, next);
}

void FakeServerTestCase::disconnectedSocketTest()  {
    fakeServer->addPendingConnection(fakeSocket);

    fakeServer->disconnectedSocket(fakeSocket);

    CPPUNIT_ASSERT_EQUAL(fakeSocket->state(), SocketState::UnconnectedState);
}
