#include "logger.h"

using namespace std;

const string Logger::Debug = "DEBUG";
const string Logger::Info  = "INFO";
const string Logger::Error = "ERROR";

const char* const Logger::LogFileName = "catchlog.txt";

Logger* Logger::pInstance = nullptr;

mutex Logger::sMutex;

Logger& Logger::instance()
{
    static Cleanup cleanup;

    lock_guard<mutex> guard(sMutex);
    if (pInstance == nullptr) {
        pInstance = new Logger();
    }

    return *pInstance;
}

Logger::Cleanup::~Cleanup()
{
    lock_guard<mutex> guard(Logger::sMutex);
    delete Logger::pInstance;
    Logger::pInstance = nullptr;
}

Logger::~Logger()
{
    m_OutputStream.close();
}

Logger::Logger()
{
    m_OutputStream.open(LogFileName, ios_base::app);
    if (!m_OutputStream.good()) {
        throw runtime_error("Unable to initialize the Logger!");
    }
}

void Logger::log(const string& inLogLevel, const string& inMessage)
{
    lock_guard<mutex> guard(sMutex);
    logHelper(inMessage, inLogLevel);
}

void Logger::log(const string& inLogLevel, const vector<string>& inMessages)
{
    lock_guard<mutex> guard(sMutex);
    for (size_t i = 0; i < inMessages.size(); i++) {
        logHelper(inLogLevel, inMessages[i]);
    }
}

void Logger::logHelper(const std::string& inLogLevel, const std::string& inMessage)
{
    m_OutputStream << inLogLevel << ": " << inMessage << endl;
}
