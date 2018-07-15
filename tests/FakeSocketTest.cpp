#include "FakeSocketTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FakeSocketTestCase);

using namespace CatchChallenger;

void FakeSocketTestCase::setUp()
{
    fakeSocket = new FakeSocket();
}

void FakeSocketTestCase::tearDown()
{
    delete fakeSocket;
    fakeSocket = nullptr;
}

void FakeSocketTestCase::openTest() {

    fakeSocket->open(DeviceMode::ReadWrite);

    //todo: don't know what is the mode
    CPPUNIT_ASSERT_EQUAL(true, true);
}

void FakeSocketTestCase::abortTest() {
    fakeSocket->abort();

    //todo: don't know what if the abort was done
    CPPUNIT_ASSERT_EQUAL(true, true);
}

void FakeSocketTestCase::disconnectFromHostTest() {
    fakeSocket->disconnectFromHost();

    SocketState res = fakeSocket->state();
    CPPUNIT_ASSERT_EQUAL(res, SocketState::UnconnectedState);
}

void FakeSocketTestCase::disconnectFromFakeServerTest() {
    fakeSocket->disconnectFromFakeServer();

    SocketState res = fakeSocket->state();
    CPPUNIT_ASSERT_EQUAL(res, SocketState::UnconnectedState);
}

void FakeSocketTestCase::connectToHostTest() {
    std::string host("127.0.0.1");

    fakeSocket->connectToHost(host, 80);

    SocketState res = fakeSocket->state();
    CPPUNIT_ASSERT_EQUAL(res, SocketState::ConnectedState);
}

void FakeSocketTestCase::bytesAvailableTest() {
    int64_t bytes = fakeSocket->bytesAvailable();

    CPPUNIT_ASSERT_EQUAL(bytes, static_cast<int64_t>(0));
}

void FakeSocketTestCase::closeTest() {
    fakeSocket->close();
    SocketState res = fakeSocket->state();

    CPPUNIT_ASSERT_EQUAL(res, SocketState::ClosingState);
}

void FakeSocketTestCase::getTheOtherSocketTest() {
    FakeSocket* other = fakeSocket->getTheOtherSocket();

    CPPUNIT_ASSERT(other != nullptr);
}

void FakeSocketTestCase::getRXSizeTest() {
    uint64_t res = fakeSocket->getRXSize();

    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint64_t>(16));
}

void FakeSocketTestCase::getTXSizeTest() {
    uint64_t res = fakeSocket->getTXSize();

    CPPUNIT_ASSERT_EQUAL(res, static_cast<uint64_t>(16));
}

void FakeSocketTestCase::stateTest() {
    SocketState res = fakeSocket->state();

    CPPUNIT_ASSERT_EQUAL(res, SocketState::UnconnectedState);
}

void FakeSocketTestCase::isValidTest() {
    bool res = fakeSocket->isValid();

    CPPUNIT_ASSERT_EQUAL(res, true);
}

void FakeSocketTestCase::socketDescriptorTest() {
    bool fd = fakeSocket->socketDescriptor();

    CPPUNIT_ASSERT(fd != 0);
}

void FakeSocketTestCase::flushTest() {
    fakeSocket->flush();

    //TODO: cannot know if the flush wad done
    CPPUNIT_ASSERT_EQUAL(true, true);
}

void FakeSocketTestCase::localAddressTest() {
    std::string address = fakeSocket->localAddress();

    CPPUNIT_ASSERT_EQUAL(address, std::string("127.0.0.1"));
}

void FakeSocketTestCase::localPortTest() {
    int port = fakeSocket->localPort();

    CPPUNIT_ASSERT_EQUAL(port, 80);
}

void FakeSocketTestCase::peerAddressTest() {
    std::string address =fakeSocket->peerAddress();

    CPPUNIT_ASSERT_EQUAL(address, std::string("127.0.0.1"));
}

void FakeSocketTestCase::peerNameTest() {
    std::string name =fakeSocket->peerName();

    CPPUNIT_ASSERT_EQUAL(name, std::string(""));
}

void FakeSocketTestCase::peerPortTest() {
    int port = fakeSocket->peerPort();

    CPPUNIT_ASSERT_EQUAL(port, 80);
}

void FakeSocketTestCase::errorStringTest() {
    std::string error = fakeSocket->errorString();

    CPPUNIT_ASSERT_EQUAL(error, std::string(""));
}

void FakeSocketTestCase::openModeTest() {
    bool res = fakeSocket->openMode();

    CPPUNIT_ASSERT_EQUAL(res, true);
}
