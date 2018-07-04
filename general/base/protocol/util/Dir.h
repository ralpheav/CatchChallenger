#ifndef CATCHCHALLENGER_DIR_H
#define CATCHCHALLENGER_DIR_H

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include "limits.h"
#include "stdlib.h"
#include <string>

namespace CatchChallenger
{
    class Dir
    {
        private:
            std::string m_path;

        public:

            Dir() {
            }

            Dir(const std::string& path) {
                this->m_path = path;
            }

            void mkpath(const std::string& path) {
                struct stat st = {0};
                if (stat(path.c_str(), &st) == -1) {
                    mkdir(path.c_str(), 0700);
                }
            }

            void mkpath() {
                if (this->m_path.empty()) {
                    this->m_path = currentPath();
                }
                mkpath(this->m_path);
            }

            std::string path() {
                return m_path;
            }

            std::string absolutePath() {
                return m_path;
            }

            std::string currentPath() {

                std::string apath = realpath(__FILE__, NULL);
                return apath;
            }

            static std::string appPath() {
                std::string appdir;

                #ifdef _WIN32
                    appdir = getenv("APPDATA");
                    if (appdir.empty()) {
                        appdir = getenv("USER");
                        appdir.append("\\local\\appData");
                    }
                #else
                    #ifdef __linux__
                        struct passwd password;
                        size_t bufferSize = sysconf(_SC_GETPW_R_SIZE_MAX);
                        char* buffer = new char[bufferSize];
                        struct passwd* passwordResult;
                        int result = getpwuid_r(getuid(), &password, buffer, bufferSize, &passwordResult);
                        appdir = passwordResult->pw_dir;
                        if (result == 0) {
                            //appdir.append("/");
                        }
                    #endif
                #endif

                return appdir;
            }
    };
}

#endif // CATCHCHALLENGER_DIR_H
