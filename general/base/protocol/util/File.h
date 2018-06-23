#ifndef FILE_H
#define FILE_H

#include <fstream>
#include <iostream>
#include <sys/stat.h>

namespace CatchChallenger
{
    enum FileMode {
        ReadOnly,
        WriteOnly,
    };

    class File
    {
        private:
            std::string filename;
            ofstream outfile;
            ifstream infile;

        public:

            File() {
            }

            File(const std::string& filename, FileMode mode = FileMode::WriteOnly) {
                this->filename = filename;
                open(mode);
            }

            void open(FileMode mode) {
                if (mode == FileMode::WriteOnly) {
                    outfile.open(this->filename);
                }
                if (mode == FileMode::ReadOnly) {
                    infile.open(this->filename);
                }
            }

            void setFileName(const std::string& filename) {
                this->filename = filename;
            }

            std::string filename() {
                return filename;
            }

            bool exists() {
                ifstream f(this->filename);
                return f.good();
            }

            void write(const std::string& data) {
                if (mode == FileMode::WriteOnly) {
                    outfile << data << endl;
                }
            }

            template<class T>
            void write(T data) {
                if (mode == FileMode::WriteOnly) {
                    outfile.write(static_cast<T>(data), sizeof(data));
                }
            }

            void write(int data) {
                if (mode == FileMode::WriteOnly) {
                    outfile << data << endl;
                }
            }

            void write(float data) {
                if (mode == FileMode::WriteOnly) {
                    outfile << data << endl;
                }
            }

            void close() {
                if (mode == FileMode::WriteOnly) {
                    outfile.close();
                }

                if (mode == FileMode::ReadOnly) {
                    infile.close();
                }
            }

            template<class T>
            T readAll() {
                T data;
                if (mode == FileMode::ReadOnly) {
                    infile >> data;
                }
                return data;
            }

            template<class T>
            void read(T& data) {
                if (mode == FileMode::ReadOnly) {
                    outfile.read(static_cast<T>(data), sizeof(data));
                }
            }

            void read(std::string& data) {
                if (mode == FileMode::ReadOnly) {
                    infile >> data;
                }
            }

            void read(int& data) {
                if (mode == FileMode::ReadOnly) {
                    infile >> data;
                }
            }

            void read(float& data) {
                if (mode == FileMode::ReadOnly) {
                    infile >> data;
                }
            }

            void remove() {
                if (mode == FileMode::WriteOnly) {
                    outfile << "" << endl;
                }
            }
    }
}

#endif // FILE_H
