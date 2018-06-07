#ifndef CATCHCHALLENGER_LOGGER_H
#define CATCHCHALLENGER_LOGGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include <stdexcept>

class Logger
{
    public:
        static const std::string Debug;
        static const std::string Info;
        static const std::string Error;

    public:
	static Logger& instance();
        void log(const std::string& inLogLevel, const std::string& inMessage);
        void log(const std::string& inLogLevel, const std::vector<std::string>& inMessages);

    protected:

        static Logger* pInstance;
        static const char* const LogFileName;
        std::ofstream m_OutputStream;
        friend class Cleanup;

        class Cleanup
	{
            public:
		~Cleanup();
	};

        void logHelper(const std::string& inLogLevel, const std::string& inMessage);

    private:
        static std::mutex sMutex;

    private:
        Logger();
        Logger(const Logger&);
	Logger& operator=(const Logger&);
        virtual ~Logger();
};

#endif // LOGGER_H
