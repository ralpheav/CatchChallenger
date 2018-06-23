#ifndef FILE_H
#define FILE_H

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
            std::string path;

        public:

            Dir() {
            }

            Dir(const std::string& path) {
                this->path = path;
            }

            void mkpath(const std::string& path) {
                struct stat st = {0};
                if (stat(path.c_str(), &st) == -1) {
                    mkdir(path.c_str(), 0700);
                }
            }

            std::string path() {
                return path;
            }

            std::string absolutePath() {
                std::string apath = realpath(__FILE, NULL);
                return apath;
            }
    }
}

#endif // FILE_H
