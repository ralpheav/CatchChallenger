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

            Dir();

            Dir(const std::string& path);

            void mkpath(const std::string& path);

            void mkpath();

            std::string path();

            std::string absolutePath();

            std::string currentPath();

            static std::string appPath();
    };
}

#endif // CATCHCHALLENGER_DIR_H
