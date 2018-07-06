#ifndef CATCHCHALLENGER_NETWORKPROXY_H
#define CATCHCHALLENGER_NETWORKPROXY_H

#include <iostream>
#include <string>

namespace CatchChallenger
{

    /**
     * @brief The NetworkProxy class, have nothing todo, yet...
     */
    class NetworkProxy
    {
        public:
            enum ProxyType {
                DefaultProxy,
                Socks5Proxy,
                NoProxy,
                HttpProxy,
                HttpCachingProxy,
                FtpCachingProxy
            };

            static NetworkProxy m_networkProxy;

        private:

            ProxyType m_type;

        public:
            explicit NetworkProxy() {
                m_type = DefaultProxy;
            }

            ~NetworkProxy() {
            }

            ProxyType type() const {
                return m_type;
            }

            static void setApplicationProxy(const NetworkProxy& proxy) {
                m_networkProxy = proxy;
            }

            static NetworkProxy applicationProxy() {
                return m_networkProxy;
            }

    };

    NetworkProxy NetworkProxy::m_networkProxy;
}

#endif // CATCHCHALLENGER_NETWORKPROXY_H
