#ifndef CATCHCHALLENGER_API_CLIENT_VIRTUAL_H
#define CATCHCHALLENGER_API_CLIENT_VIRTUAL_H

#include <string>

#include "../../general/base/ConnectedSocket.h"
#include "Api_protocol.h"

namespace CatchChallenger {

    class Settings {

        private:
            std::string m_appPath;
            std::string m_datapackDir;
            std::string m_host;
            std::string m_port;

        public:
            Settings& setAppPath(const std::string& appPath) {
                m_appPath = appPath;
                return *this;
            }

            std::string getAppPath() const {
                return m_appPath;
            }

            Settings& setDatapackDir(const std::string& datapackDir) {
                m_datapackDir = datapackDir;
                return *this;
            }

            std::string getDatapackDir() const {
                return m_datapackDir;
            }

            Settings& setHost(const std::string& host) {
                m_host = host;
                return *this;
            }

            std::string getHost() const {
                return m_host;
            }

            Settings& setPort(const std::string& port) {
                m_port = port;
                return *this;
            }

            std::string getPort() const {
                return m_port;
            }
    };

    class Api_client_virtual : public Api_protocol
    {
        public:
            explicit Api_client_virtual(ConnectedSocket *socket, const Settings& settings);
            ~Api_client_virtual();
            void sendDatapackContentBase(const std::string &hashBase=std::string());
            void sendDatapackContentMainSub(const std::string &hashMain=std::string(),const std::string &hashSub=std::string());
            void tryDisconnect();
        protected:
            //general data
            void defineMaxPlayers(const uint16_t &);
    };
}

#endif // Protocol_and_virtual_data_H
