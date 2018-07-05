#ifndef CATCHCHALLENGER_ZSTDDECODETHREAD_H
#define CATCHCHALLENGER_ZSTDDECODETHREAD_H

#include <thread>
#include <chrono>
#include <vector>
#include <string>

#include "DatapackChecksum.h"

namespace CatchChallenger
{
    /**
     * @brief The ZstdDecodeThread class, useful
     */
    class ZstdDecodeThread
    {
            std::thread the_thread;
            bool stop_thread;
            unsigned int m_sleepTime;
            std::vector<char> (*the_function)(const std::string&);
            std::string m_parameter_of_function;
            DatapackChecksum::FullDatapackChecksumReturn (*the_fullfunction)(const std::string&);
            std::string m_parameter_of_fullfunction;
            std::vector<char> result;
            DatapackChecksum::FullDatapackChecksumReturn fullResult;

        public:
            explicit ZstdDecodeThread() : the_thread() {
                stop_thread = false;
                m_sleepTime = 1;
                m_parameter_of_function = std::string();
                m_parameter_of_fullfunction = std::string();
            }

            ~ZstdDecodeThread() {
                exit();
                quit();
            }

            void setFunction(std::vector<char> (*parameter_function)(const std::string&), std::string parameter) {
                the_function = parameter_function;
                m_parameter_of_function = parameter;
            }

            void setFunction(DatapackChecksum::FullDatapackChecksumReturn (*parameter_function)(const std::string&), std::string parameter) {
                the_fullfunction = parameter_function;
                m_parameter_of_fullfunction = parameter;
            }

            void setSleepTime(int time) {
                m_sleepTime = time;
            }

            void decode() {
                while (!stop_thread) {
                    if (!m_parameter_of_function.empty()) {
                        result = (*the_function)(m_parameter_of_function);
                    }

                    if (!m_parameter_of_fullfunction.empty()) {
                        fullResult = (*the_fullfunction)(m_parameter_of_fullfunction);
                    }

                    std::this_thread::sleep_for(std::chrono::seconds(m_sleepTime));
                }
            }

            void decodedIsFinish() {
                //TODO
            }

            void start() {
                the_thread = std::thread(&ZstdDecodeThread::decode, this);
            }

            void exit() {
                stop_thread = false;
            }

            void quit() {
                if (the_thread.joinable()) {
                    the_thread.join();
                }
            }

            void wait() {
                std::this_thread::sleep_for(std::chrono::seconds(m_sleepTime));
            }

    };
}

#endif // CATCHCHALLENGER_ZSTDDECODETHREAD_H
