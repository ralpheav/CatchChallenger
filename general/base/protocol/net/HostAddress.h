#ifndef CATCHCHALLENGER_HOSTADDRESS_H
#define CATCHCHALLENGER_HOSTADDRESS_H

#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include <vector>
#include <string>
#include <sstream>

namespace CatchChallenger {

class HostAddress {

    static std::string localhost;
    static std::string Null;

    std::string ip;

public:

    void setAddress(const std::string& ip) {
        this->ip = ip;
    }

    void clear() {
        ip.clear();
    }

    bool isEqual(HostAddress second) const {
        return ip == second.ip;
    }

    bool isMulticast() const {
        return false;
    }

    bool isLoopback() const {
        return false;
    }

    std::string toString() const {
        return ip;
    }

    std::string toIpv6() {
        std::vector<std::string> octets = split(ip, ".");

        unsigned char* octetBytes = new unsigned char[4];

        for (int index = 0; index < 4; ++index) {
             octetBytes[i] = static_cast<unsigned char>(octets[index]);
        }

        unsigned char* ipv4asIpV6addr = new unsigned char[16];

        ipv4asIpV6addr[10] = (unsigned char)0xff;
        ipv4asIpV6addr[11] = (unsigned char)0xff;
        ipv4asIpV6addr[12] = octetBytes[0];
        ipv4asIpV6addr[13] = octetBytes[1];
        ipv4asIpV6addr[14] = octetBytes[2];
        ipv4asIpV6addr[15] = octetBytes[3];
    }

private:

    std::vector<std::string> split(const std::string& target, char delimitator)
    {
        std::vector<std::string> result;
        std::istringstream iss(target);

        for (std::string token; std::getline(iss, token, delimitator);)
        {
            result.push_back(std::move(token));
        }

        return result;
    }
};

#endif // ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
#endif // CATCHCHALLENGER_CONNECTEDSOCKET_H
