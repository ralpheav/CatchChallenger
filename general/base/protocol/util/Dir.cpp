#include "Dir.h"

using namespace CatchChallenger;

Dir::Dir() {
    this->m_path = this->currentPath();
}

Dir::Dir(const std::string& path) {
     this->m_path = path;
}

void Dir::mkpath(const std::string& path) {
    struct stat st = {0};
    if (stat(path.c_str(), &st) == -1) {
        mkdir(path.c_str(), 0700);
    }
}

void Dir::mkpath() {
    if (this->m_path.empty()) {
        this->m_path = currentPath();
    }
    mkpath(this->m_path);
}

std::string Dir::path() {
    return m_path;
}

std::string Dir::absolutePath() {
    return m_path;
}

std::string Dir::currentPath() {
    unsigned int len = 1024;
    char buffer[1024];
    int bytes;
#if defined(_WIN32) || defined(_WIN64)
    bytes = GetModuleFileName(nullptr, buffer, len);
#elif __linux__
    char temp[32];
    sprintf(temp, "proc/%d/exe", getpid());
    bytes = std::min(static_cast<unsigned int>(readlink(temp, buffer, len)), len - 1);
#else
    std::string apath = realpath(__FILE__, NULL);
    return apath;
#endif
    if (bytes == 0) {
        return std::string();
    }
    buffer[bytes] = 0;
    return buffer;
}

std::string Dir::appPath() {
    std::string appdir;

    #if defined(_WIN32) || defined(_WIN64)
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
