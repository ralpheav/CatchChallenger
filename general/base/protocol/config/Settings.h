#ifndef CATCHCHALLENGER_SETTINGS_H
#define CATCHCHALLENGER_SETTINGS_H

namespace CatchChallenger
{
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
}

#endif // GENERALVARIABLE_H
