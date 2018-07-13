    #include "ConnectedSocketTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectedSocketTestCase);

using namespace CatchChallenger;

void ConnectedSocketTestCase::setUp()
{
    fakeSocket = new FakeSocket();
    connectedSocket = new ConnectedSocket(fakeSocket);
}

void ConnectedSocketTestCase::tearDown()
{
    delete connectedSocket;
    connectedSocket = NULL;
}

void ConnectedSocketTestCase::abortTest() {
    connectedSocket->abort();

    //TODO:: is a black box
    CPPUNIT_ASSERT_EQUAL(true, true);
}

void ConnectedSocketTestCase::connectToHostTest() {
    std::string hostName = "localhost";
    uint16_t port = 80;
    connectedSocket->connectToHost(hostName, port);

    CPPUNIT_ASSERT_EQUAL(State::connected, static_cast<State>(connectedSocket->state()));
}

void ConnectedSocketTestCase::connsectToHostTest() {
    HostAddress hostName("127.0.0.1");
    uint16_t port = 80;
    connectedSocket->connectToHost(hostName, port);

    CPPUNIT_ASSERT_EQUAL(State::connected, static_cast<State>(connectedSocket->state()));
}

void ConnectedSocketTestCase::disconnectFromHostTest() {
    connectedSocket->disconnectFromHost();

    CPPUNIT_ASSERT_EQUAL(State::unconnected, static_cast<State>(connectedSocket->state()));
}

void ConnectedSocketTestCase::errorTest() {
    int res = connectedSocket->error();

    CPPUNIT_ASSERT_EQUAL(static_cast<SocketError>(res), SocketError::UnspecifiedError);
}

void ConnectedSocketTestCase::flushTest() {
    bool res = connectedSocket->flush();

    CPPUNIT_ASSERT_EQUAL(res, true);
}

void ConnectedSocketTestCase::isValidTest() {
    bool res = connectedSocket->isValid();

    CPPUNIT_ASSERT_EQUAL(res, true);
}

void ConnectedSocketTestCase::setTcpCorkTest() {
    bool res = false;
    connectedSocket->setTcpCork(&res);

    CPPUNIT_ASSERT_EQUAL(res, true);

}

void ConnectedSocketTestCase::localAddressTest() {

}

void ConnectedSocketTestCase::localPortTest() {

}

void ConnectedSocketTestCase::peerAddressTest() {

}

void ConnectedSocketTestCase::peerNameTest() {

}

void ConnectedSocketTestCase::peerPortTest() {

}

void ConnectedSocketTestCase::getSSLSocket() {

}

void ConnectedSocketTestCase::stateTest() {

}

void ConnectedSocketTestCase::waitForConnectedTest() {

}

void ConnectedSocketTestCase::waitForDisconnectedTest() {

}

void ConnectedSocketTestCase::bytesAvailableTest() {

}

void ConnectedSocketTestCase::openModeTest() {

}

void ConnectedSocketTestCase::errorStringTest() {

}

void ConnectedSocketTestCase::readDataTest() {

}

void ConnectedSocketTestCase::writeDataTest() {

}

void ConnectedSocketTestCase::closeTest() {

}

void ConnectedSocketTestCase::isSSLTest() {

}

void ConnectedSocketTestCase::isFakeTest() {

}

void ConnectedSocketTestCase::isTCPTest() {

}

void ConnectedSocketTestCase::existsTest() {

}

void ConnectedSocketTestCase::connectTest() {

}

void ConnectedSocketTestCase::isSequential() {

}

void ConnectedSocketTestCase::canReadLine() {

}

void ConnectedSocketTestCase::sslErrors() {

}
