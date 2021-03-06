#ifndef CATCHCHALLENGER_HOSTADDRESS_H
#define CATCHCHALLENGER_HOSTADDRESS_H

#if ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

namespace CatchChallenger
{

    class HostAddress {

        std::string ip;

    public:

        enum SpecialAddress {
            Null,
            Broadcast,
            LocalHost,
            LocalHostIPv6,
            Any,
            AnyIPv6,
            AnyIPv4
        };

        HostAddress() {
        }

        HostAddress(std::string ip) {
            this->ip = ip;
        }

        HostAddress(SpecialAddress address) {
            setAddress(address);
        }

        void setAddress(const std::string& ip) {
            this->ip = ip;
        }

        void setAddress(SpecialAddress address) {
            switch(address) {
                Null:
                        this->ip = std::string("0.0.0.0");
                        break;
                LocalHost:
                        this->ip = std::string("127.0.0.1");
                        break;
                Broadcast:
                LocalHostIPv6:
                Any:
                AnyIPv6:
                AnyIPv4:
                        break;
                default:
                        break;
            }
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

        std::string hexStr(unsigned char* data, int pos, int len)
        {
            std::stringstream ss;
            ss << std::hex;

            for (int index = pos; index < pos + len; ++index) {
                ss << std::setw(2) << std::setfill('0') << (int)data[index];
                if (index + 1 < len) {
                    ss << ":";
                }
            }
            return ss.str();
        }

        std::string toIpv6() {
            std::vector<std::string> octets = split(ip, '.');

            unsigned char* octetBytes = new unsigned char[4];

            for (int index = 0; index < 4; ++index) {
                 octetBytes[index] = static_cast<unsigned char>(atoi(octets[index].c_str()));
            }

            unsigned char* ipv4asIpV6addr = new unsigned char[16];

            ipv4asIpV6addr[10] = (unsigned char)0xff;
            ipv4asIpV6addr[11] = (unsigned char)0xff;
            ipv4asIpV6addr[12] = octetBytes[0];
            ipv4asIpV6addr[13] = octetBytes[1];
            ipv4asIpV6addr[14] = octetBytes[2];
            ipv4asIpV6addr[15] = octetBytes[3];

            return hexStr(ipv4asIpV6addr, 10, 6);
        }

        HostAddress& operator = (SpecialAddress address){
            this->setAddress(address);
            return *this;
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
}

#endif // ! defined(EPOLLCATCHCHALLENGERSERVER) && ! defined (ONLYMAPRENDER)
#endif // CATCHCHALLENGER_CONNECTEDSOCKET_H
