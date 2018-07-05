#ifndef CATCHCHALLENGER_NETWORKACCESSMANAGER_H
#define CATCHCHALLENGER_NETWORKACCESSMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include "NetworkProxy.h"

namespace CatchChallenger
{
    /**
     * @brief The NetworkAccessManager class, also, need to set what is goint to do with QObject????
     */
    class NetworkAccessManager
    {
        int m_time;
        NetworkProxy m_proxy;

    public:
        explicit NetworkAccessManager() {
            m_time = 100;
        }

        explicit NetworkAccessManager(int time) {
            m_time = time;
        }

        ~NetworkAccessManager() {
        }

        void setProxy(const &proxy) const {
            m_proxy = proxy;
        }

    };
}

#endif // CATCHCHALLENGER_NETWORKACCESSMANAGER_H
