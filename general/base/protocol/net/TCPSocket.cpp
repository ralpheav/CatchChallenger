#include "TCPSocket.h"

TCPSocket::TCPSocket() {
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (!haveSocket())
    {
        std::cerr << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

TCPSocket::TCPSocket(int file_descriptor) {
    socket_file_descriptor = file_descriptor;
}

bool TCPSocket::haveSocket() const {
    return socket_file_descriptor > 0;
}

int TCPSocket::read() {
    if (!haveSocket()) {
        std::cerr << "no socket" << std::endl;
    }

    int result = ::read(socket_file_descriptor, buffer, 1024);

    return result;
}

int TCPSocket::send(const std::string& message) {
    if (!haveSocket()) {
        std::cerr << "no socket" << std::endl;
    }

    int result = ::send(socket_file_descriptor, message.c_str(), message.size(), 0);

    return result;
}

void TCPSocket::setSocketOption(SocketOption option, int parameter) {
    this->option = option;
    //getsockopt() //SO_ERROR
}

bool TCPSocket::bytesAvailable() {
    int bytes_available = 0;
    ioctl(socket_file_descriptor, FIONREAD, &bytes_available);

    return bytes_available;
}

bool TCPSocket::encryptedBytesAvailable() {
    return false;
}

void TCPSocket::abort() {
    shutdown(socket_file_descriptor, SHUT_RDWR);
}

int TCPSocket::connectToHost(const std::string& host, int port) {
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported" << std::endl;
        return 0;
    }

    strcpy(this->host, host.c_str());
    this->port = port;

    return ::connect(socket_file_descriptor, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
}

void TCPSocket::disconnectFromHost() {
    close(socket_file_descriptor);
}

SocketError TCPSocket::error() {
    return error;
}

void TCPSocket::flush() {
    int mark = 0;
    char waste[BUFSIZ];

    tcflush(1, TCIOFLUSH);

    while (true) {
        if (ioctl(socket_file_descriptor, SIOCATMARK, &mark) < 0) {
            return;
        }
        if (mark) {
            return;
        }
        read(socket_file_descriptor, waste, sizeof(waste));
    }
}

bool TCPSocket::isValid() {
    return socket_file_descriptor > 0;
}

bool TCPSocket::socketDescriptor() {
    return socket_file_descriptor > 0;
}

std::string TCPSocket::localAddress() {
    return host;
}

int TCPSocket::localPort() {
    return port;
}

std::string TCPSocket::peerAddress() {
    //TODO connected peer
}

std::string TCPSocket::peerName() {
    //TODO connected name
}

int TCPSocket::peerPort() {
    //TODO connected port
}

SocketState TCPSocket::state() {
    return state;
}

bool TCPSocket::waitForConnected(int msec) {
    waitForConnectTime = msec;
    return true;
}

bool TCPSocket::waitForDisconnected(int msec) {
    waitForDisconnectedTime = msec;
    return true;
}

bool TCPSocket::openMode() {
    //TODO
}

std::string TCPSocket::errorString() {
    //TODO
}

int TCPSocket::readData(char* message, size_t max) {

    if (!haveSocket()) {
        std::cerr << "No socket opened" << std::endl;
    }

    int result = ::read(socket_file_descriptor, message, max);
    message[max] = 0;

    return result;
}

int TCPSocket::writeData(const char* message, size_t max) {

    if (!haveSocket()) {
        std::cerr << "no socket" << std::endl;
    }

    int result = ::send(socket_file_descriptor, message, max, 0);

    return result;
}

std::string TCPSocket::getBuffer() {
    return buffer;
}
