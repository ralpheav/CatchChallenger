#ifndef CATCHCHALLENGER_FILE_H
#define CATCHCHALLENGER_FILE_H

#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <sys/stat.h>

namespace CatchChallenger
{
    enum FileMode {
        ReadOnly,
        WriteOnly,
    };

    class File
    {
            FileMode m_mode;
            std::string m_filename;
            std::fstream m_file;
            std::string m_lastError;

        public:

            File();
            File(const std::string& filename, FileMode mode = FileMode::WriteOnly);

            bool open(FileMode mode);
            bool isOpen();
            FileMode mode();
            void setFileName(const std::string& filename);
            std::string filename() const;
            bool exists();
            void write(const std::string& data);
            template<typename T>
            void write(T data) {
                if (m_mode == FileMode::WriteOnly) {
                    m_file.write(reinterpret_cast<char*>(&data), sizeof(T));
                } else {
                    std::cerr << "the file is write only" << std::endl;
                }
            }
            void write(int data);
            void write(float data);
            void close();
            std::string readAll();
            //template<typename T> void read(T& data);
            template<typename T>
            void read(T& data) {
                if (m_mode == FileMode::ReadOnly) {
                    m_file.read(reinterpret_cast<char*>(&data), sizeof(T));
                } else {
                    std::cerr << "the file is read only" << std::endl;
                }
            }
            void read(std::string& data);
            void read(int& data);
            void read(float& data);
            bool remove();
            bool deleteFile();
            std::string errorString();
    };
}

#endif // CATCHCHALLENGER_FILE_H
