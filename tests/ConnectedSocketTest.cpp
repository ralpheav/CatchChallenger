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

void ConnectedSocketTestCase::connectToHost2Test() {
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

    CPPUNIT_ASSERT_EQUAL(static_cast<SocketError>(res), SocketError::UnknownSocketError);
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
    HostAddress address = connectedSocket->localAddress();

    CPPUNIT_ASSERT_EQUAL(address.toString(), std::string("127.0.0.1"));
}

void ConnectedSocketTestCase::localPortTest() {
    uint16_t port = connectedSocket->localPort();

    CPPUNIT_ASSERT_EQUAL(port, static_cast<uint16_t>(80));
}

void ConnectedSocketTestCase::peerAddressTest() {
    HostAddress peerAddress = connectedSocket->peerAddress();

    CPPUNIT_ASSERT_EQUAL(peerAddress.toString(), std::string("127.0.0.1"));
}

void ConnectedSocketTestCase::peerNameTest() {
    std::string peerName = connectedSocket->peerName();

    CPPUNIT_ASSERT_EQUAL(peerName, std::string("127.0.0.1"));
}

void ConnectedSocketTestCase::peerPortTest() {
    uint16_t port = connectedSocket->peerPort();

    CPPUNIT_ASSERT_EQUAL(port, static_cast<uint16_t>(80));
}

void ConnectedSocketTestCase::getSSLSocket() {
    SSLSocket* socket = connectedSocket->getSSLSocket();

    CPPUNIT_ASSERT(socket != nullptr);
}

void ConnectedSocketTestCase::stateTest() {
    FakeSocket* fake = new FakeSocket();
    ConnectedSocket* connSocket = new ConnectedSocket(fake);

    int state = connSocket->state();

    CPPUNIT_ASSERT_EQUAL(static_cast<State>(state), State::connected);

    connSocket->close();
    state = connSocket->state();

    CPPUNIT_ASSERT_EQUAL(static_cast<State>(state), State::unconnected);
}

void ConnectedSocketTestCase::waitForConnectedTest() {
    bool result = connectedSocket->waitForConnected(3000);

    CPPUNIT_ASSERT_EQUAL(result, true);
}

void ConnectedSocketTestCase::waitForDisconnectedTest() {
    bool result = connectedSocket->waitForDisconnected(2000);

    CPPUNIT_ASSERT_EQUAL(result, true);
}

void ConnectedSocketTestCase::bytesAvailableTest() {
    bool result = connectedSocket->waitForDisconnected(2000);

    CPPUNIT_ASSERT_EQUAL(result, true);
}

void ConnectedSocketTestCase::openModeTest() {
    OpenMode result = connectedSocket->openMode();

    CPPUNIT_ASSERT_EQUAL(result, OpenMode::open);
}

void ConnectedSocketTestCase::errorStringTest() {
    std::string error = connectedSocket->errorString();

    //TODO
    CPPUNIT_ASSERT_EQUAL(error, std::string(""));
}

void ConnectedSocketTestCase::readDataTest() {
    char data[] = "";
    int64_t res = connectedSocket->readData(data, static_cast<int64_t>(32));

    CPPUNIT_ASSERT_EQUAL(res, static_cast<int64_t>(0));
    CPPUNIT_ASSERT_EQUAL(strlen(data), static_cast<size_t>(32));
}

void ConnectedSocketTestCase::writeDataTest() {
    const char* data = "This is a test..";
    int64_t res = connectedSocket->writeData(data, static_cast<int64_t>(16));

    CPPUNIT_ASSERT_EQUAL(res, static_cast<int64_t>(0));
}

void ConnectedSocketTestCase::closeTest() {
    connectedSocket->close();

    CPPUNIT_ASSERT_EQUAL(static_cast<State>(connectedSocket->state()), State::unconnected);
}

void ConnectedSocketTestCase::isSSLTest() {
    bool res = connectedSocket->isSSL();

    CPPUNIT_ASSERT_EQUAL(res, false);
}

void ConnectedSocketTestCase::isFakeTest() {
    bool res = connectedSocket->isFake();

    CPPUNIT_ASSERT_EQUAL(res, true);
}

void ConnectedSocketTestCase::isTCPTest() {
    bool res = connectedSocket->isTCP();

    CPPUNIT_ASSERT_EQUAL(res, false);
}

void ConnectedSocketTestCase::existsTest() {
    bool res = connectedSocket->exists();

    CPPUNIT_ASSERT_EQUAL(res, true);
}

void ConnectedSocketTestCase::connectTest() {
    bool res = connectedSocket->connect(SslMode::SslClientMode);

    CPPUNIT_ASSERT_EQUAL(res, true);
}

void ConnectedSocketTestCase::isSequentialTest() {
    //TODO: test protected
   // bool res = connectedSocket->isSequential();

    //CPPUNIT_ASSERT_EQUAL(res, true);
}

void ConnectedSocketTestCase::canReadLineTest() {
    //TODO: test protected
   // bool res = connectedSocket->canReadLine();

    //CPPUNIT_ASSERT_EQUAL(res, true);
}

void ConnectedSocketTestCase::sslErrorsTest() {
    //TODO: test protected
    //std::list<SslError> errors = connectedSocket->sslErrors();

    //CPPUNIT_ASSERT_EQUAL(errors.size(), 0);
}
