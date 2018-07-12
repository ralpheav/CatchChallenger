#ifndef CATCHCHALLENGER_FILE_H
#define CATCHCHALLENGER_FILE_H

#include <fstream>
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
            template<class T> void write(T data);
            void write(int data);
            void write(float data);
            void close();
            std::string readAll();
            template<class T> void read(T& data);
            void read(std::string& data);
            void read(int& data);
            void read(float& data);
            bool remove();
            bool deleteFile();
            std::string errorString();
    };
}

#endif // CATCHCHALLENGER_FILE_H
