#ifndef CATCHCHALLENGER_DIR_H
#define CATCHCHALLENGER_DIR_H

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include "limits.h"
#include "stdlib.h"

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

            std::string path() {
                return m_path;
            }

            std::string absolutePath() {

                std::string apath = realpath(__FILE__, NULL);
                return apath;
            }
    };
}

#endif // CATCHCHALLENGER_DIR_H
