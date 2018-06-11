#include "SSLServer.h"

SSLServer::SSLServer() {
    SSL_library_init();
}

SSLServer::~SSLServer() {
    //close server socket
    close(socket_descriptor);
    //release context
    SSL_CTX_free(ctx);
}

int SSLServer::openListener(int port)
{
    socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_descriptor, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        std::cerr << "can't bind port" << std::endl;
        abort();
    }

    if (listen(socket_descriptor, 10) != 0)
    {
        std::cerr << "Can't configure listening port" << std::endl;
        abort();
    }

    return socket_descriptor;
}

SSL_CTX* SSLServer::initServerCTX()
{
    const SSL_METHOD *method;

    //load and register all crypt methods
    OpenSSL_add_all_algorithms();
    //load all error messages
    SSL_load_error_strings();
    //create new server instance
    method = SSLv23_server_method();
    //create new context from method
    ctx = SSL_CTX_new(method);

    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

void SSLServer::loadCertificates(const char* CertFile, const char* KeyFile)
{
    //New lines
    if (SSL_CTX_load_verify_locations(ctx, CertFile, KeyFile) != 1)
        ERR_print_errors_fp(stderr);

    if (SSL_CTX_set_default_verify_paths(ctx) != 1)
        ERR_print_errors_fp(stderr);
    //End new lines

    //Set the local certificate from CertFile
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    //Set the private key from KeyFile (may be the same as CertFile)
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    //Verify private key
    if (!SSL_CTX_check_private_key(ctx))
    {
        std::cerr << "Private key does not match the public certificate" << std::endl;
        abort();
    }

    //New lines
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);
    //End new lines
}

void SSLServer::showCerts()
{
    X509 *cert;
    char *line;

    //Get certificates (if available)
    cert = SSL_get_peer_certificate(ssl);
    if (cert != nullptr)
    {
        std::cout << "Server certificates:" << std::endl;
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        std::cout << "Subject: " << line << std::endl;
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        std::cout << "Issuer: " << line << std::endl;
        free(line);
        X509_free(cert);
    } else {
       std::cout << "No certificates." << std::endl;
    }
}

bool SSLServer::connectClient(SSL* ssl) {
    return SSL_accept(ssl) != -1;
}

int SSLServer::readClient(SSL* ssl) {
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));
    buffer[bytes] = 0;

    return bytes;
}

const char* SSLServer::getBuffer() {
    return buffer;
}

void SSLServer::setReply(const char* input) {
    sprintf(reply, input, buffer);
}

void SSLServer::sendClient(SSL* ssl) {
    SSL_write(ssl, reply, strlen(reply));
}

void SSLServer::closeClient(SSL* ssl) {
    int sd_aux;
    //get socket connection
    sd_aux = SSL_get_fd(ssl);
    //release SSL state
    SSL_free(ssl);
    //close connection
    close(sd_aux);
}

SSL* SSLServer::accept() {
    struct sockaddr_in addr2;
    socklen_t len = sizeof(addr);

    //Accept connection as usual
    int client = ::accept(socket_descriptor, (struct sockaddr*)&addr2, &len);

    std::cout << "Connection: " << inet_ntoa(addr2.sin_addr) << ntohs(addr2.sin_port) << std::endl;

    //get new SSL state with context
    ssl = SSL_new(ctx);
    //set connection socket to SSL state
    SSL_set_fd(ssl, client);

    return ssl;
}
