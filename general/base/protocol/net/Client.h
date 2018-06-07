#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define HOST "127.0.0.1"
#define PORT 8080

class Client {

    private:
        struct sockaddr_in address;
        struct sockaddr_in serv_addr;
        int socket_handler;
        char buffer[1024] = {0}; //TODO: change for diffrent const values e.g. 1024

    public:
        Client();

    protected:
        bool haveSocket() const;

    public:
        bool create(std::string host, uint32_t port);
        bool connect();
        // TODO: change the input for an byte array
        int send(const std::string& message);
        std::string getBuffer() const;
};

#endif
