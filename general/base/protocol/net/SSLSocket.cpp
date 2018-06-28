#include "SSLSocket.h"

using namespace CatchChallenger;

SSLSocket::SSLSocket() {
    SSL_library_init();
}

SSLSocket::~SSLSocket() {
    if (m_state == SocketState::ConnectedState) {
        // release connection state
        SSL_free(ssl);
    }
    // close socket
    ::close(socket_descriptor);
    // release context
    SSL_CTX_free(ctx);
}

void SSLSocket::open(DeviceMode mode) {
    m_mode = mode;
    //TODO
}

void SSLSocket::loadCertificates(const char* CertFile, const char* KeyFile)
{
    //set the local certificate from CertFile
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    //set the private key from KeyFile (may be the same as CertFile)
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    //verify private key
    if (!SSL_CTX_check_private_key(ctx) )
    {
        std::cerr << "Private key does not match the public certificate" << std::endl;
        abort();
    }
}

int SSLSocket::openConnection(const char* hostname, int port)
{
    if ((hostAddress = gethostbyname(hostname)) == nullptr)
    {
        perror(hostname);
        abort();
    }

    socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&addr, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = *(long*)(hostAddress->h_addr);

    if (::connect(socket_descriptor, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        ::close(socket_descriptor);
        perror(hostname);
        abort();
    }

    return socket_descriptor;
}

bool SSLSocket::connect() {
    //create new SSL connection state
    ssl = SSL_new(ctx);
    //attach the socket desccriptor
    SSL_set_fd(ssl, socket_descriptor);

    if (SSL_connect(ssl) == -1) {
        m_state = SocketState::UnconnectedState;

        return false;
    }
    m_state = SocketState::ConnectedState;

    return true;
}

bool SSLSocket::getState() {
    return m_state;
}

SSL_CTX* SSLSocket::init()
{
    const SSL_METHOD *method;

    //load crypto algorithms
    OpenSSL_add_all_algorithms();
    //bring and register ssl error messages
    SSL_load_error_strings();
    //create new client instance
    method = SSLv23_client_method();
    //create new context
    ctx = SSL_CTX_new(method);

    if (ctx == nullptr) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

const char* SSLSocket::getEncryptation() {
    //get the current cypher method
    return SSL_get_cipher(ssl);
}

void SSLSocket::showCerts()
{
    X509 *cert;
    char *line;

    //Get the certificates
    cert = SSL_get_peer_certificate(ssl);

    if (cert != nullptr)
    {
        std::cout << "Server certificates:" << std::endl;
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        std::cout << "Subject: " << line << std::endl;
        //free the line
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        std::cout << "Issuer: " << line << std::endl;
        free(line);
        //free the certificates memory allocation
        X509_free(cert);
    } else {
        std::cout << "No certificates." << std::endl;
    }
}

int SSLSocket::send(const char* message) {
    // encrypt and send the message
    return SSL_write(ssl, message, strlen(message));
}

int SSLSocket::read() {
    int bytes;
    //get the reply decrypted message
    bytes = SSL_read(ssl, buffer, sizeof(buffer));
    buffer[bytes] = 0;

    return bytes;
}

void SSLSocket::close() {
    SSL_free(ssl);
    ::close(socket_descriptor);
    SSL_CTX_free(ctx);
}

uint64_t SSLSocket::getRXSize() {
    //TODO
    return 0;
}

uint64_t SSLSocket::getTXSize() {
    //TODO
    return 0;
}

int64_t SSLSocket::bytesAvailable() const {
    int bytes_available = 0;
    ioctl(socket_descriptor, FIONREAD, &bytes_available);

    return bytes_available;
}

bool SSLSocket::encryptedBytesAvailable() {
    return this->getEncryptation() != nullptr;
}

void SSLSocket::abort() {
    shutdown(socket_descriptor, SHUT_RDWR);
}

void SSLSocket::connectToHost(const std::string& host, int port) {
    this->openConnection(host.c_str(), port);

    strcpy(this->host, host.c_str());
    this->port = port;

    if (this->connect()) {
        m_state = SocketState::ConnectedState;
    }
}

void SSLSocket::disconnectFromHost() {
    ::close(socket_descriptor);
    SSL_CTX_free(ctx);
}

void SSLSocket::flush() {
    int mark = 0;
    char waste[BUFSIZ];

    tcflush(1, TCIOFLUSH);

    while (true) {
        if (ioctl(socket_descriptor, SIOCATMARK, &mark) < 0) {
            return;
        }
        if (mark) {
            return;
        }
        ::read(socket_descriptor, waste, sizeof(waste));
    }
}

bool SSLSocket::isValid() const {
    return socket_descriptor > 0;
}

bool SSLSocket::isSequential() const {
    //TODO
    return false;
}

bool SSLSocket::canReadLine() const {
    //TODO
    return false;
}

uint64_t SSLSocket::readData(char* data, int64_t maxSize) {
    //TODO
    return 0;
}

uint64_t SSLSocket::writeData(const char* data, int64_t maxSize) {
    //TODO
    return 0;
}

void SSLSocket::internal_writeData(std::vector<unsigned char> rawData) {
    //TODO
}

int64_t SSLSocket::bytesAvailableWithMutex() {
    //TODO
    return 0;
}

bool SSLSocket::socketDescriptor() {
    return socket_descriptor > 0;
}

std::string SSLSocket::localAddress() {
    return host;
}

int SSLSocket::localPort() {
    return port;
}

std::string SSLSocket::peerAddress() {
    //TODO
}

std::string SSLSocket::peerName() {
    //TODO
}

int SSLSocket::peerPort() {
    //TODO
}

void SSLSocket::setSslMode(SslMode mode) {
    this->m_sslMode = mode;
}

SslMode SSLSocket::sslMode() {
    return m_sslMode;
}

void SSLSocket::setPeerVerifyMode(PeerVerifyMode mode) {
    verifyMode = mode;
}

void SSLSocket::ignoreSslErrors() {
    ignoreErrors = true;
}

void SSLSocket::startClientEncryption() {
    SSL_library_init();
}

bool SSLSocket::waitForConnected(int msec) {
    waitForConnectTime = msec;
    return true;
}

bool SSLSocket::waitForDisconnected(int msec) {
    waitForDisconnectedTime = msec;
    return true;
}

bool SSLSocket::openMode() {
    //TODO
    return true;
}

std::string SSLSocket::errorString() {
    //TODO
    return std::string();
}

int SSLSocket::readData(char* message, size_t max) {
    int bytes;

    bytes = SSL_read(ssl, message, max);
    message[max] = 0;

    return bytes;
}

int SSLSocket::writeData(const char* message, size_t max) {

    int result = SSL_write(ssl, message, max);

    return result;
}

const char* SSLSocket::getBuffer() {
   return buffer;
}

SSLInfo SSLSocket::peerCertificate() {
    //TODO: get SSL certificate info
    return info;
}
