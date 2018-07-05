#ifndef CATCHCHALLENGER_NETWORKREPLY_H
#define CATCHCHALLENGER_NETWORKREPLY_H

#include <iostream>
#include <vector>
#include <string>

namespace CatchChallenger
{
    /**
     * @brief The NetworkReply class, have nothing todo, yet...
     */
    class NetworkReply
    {
        std::string response;
        std::vector<unsigned char> m_reply;
    public:
        explicit NetworkReply() {

        }
        ~NetworkReply() {

        }

        std::string getReply() const {
            return response;
        }

        std::vector<unsigned char> reply() const{
            return m_reply;
        }
    };
}

#endif // CATCHCHALLENGER_NETWORKREPLY_H
