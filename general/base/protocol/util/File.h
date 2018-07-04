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
        private:
            FileMode m_mode;
            std::string m_filename;
	    std::ofstream outfile;
	    std::ifstream infile;

        public:

            File() {
                m_mode = ReadOnly;
            }

            File(const std::string& filename, FileMode mode = FileMode::WriteOnly) {
                m_filename = filename;
		m_mode = mode; 
                open(mode);
            }

            bool open(FileMode mode) {
		m_mode = mode;
                if (m_mode == FileMode::WriteOnly) {
                    outfile.open(this->m_filename);
                    return outfile.is_open();
                }
                if (m_mode == FileMode::ReadOnly) {
                    infile.open(this->m_filename);
                    return infile.is_open();
                }
                return false;
            }

            void setFileName(const std::string& filename) {
                this->m_filename = filename;
            }

            std::string filename() const {
                return m_filename;
            }

            bool exists() {
		std::ifstream f(this->m_filename);
                return f.good();
            }

            void write(const std::string& data) {
                std::cout <<"STRING" << std::endl;
                if (this->m_mode == FileMode::WriteOnly) {
                    outfile << data << std::endl;
                } else {
                    std::cerr << "the file is write only" << std::endl;
                }
            }

            template<class T>
            void write(T data) {
                if (m_mode == FileMode::WriteOnly) {
                    outfile.write(static_cast<T>(data), sizeof(data));
                } else {
                    std::cerr << "the file is write only" << std::endl;
                }

            }

            void write(int data) {
                if (m_mode == FileMode::WriteOnly) {
                    outfile << data << std::endl;
                } else {
                    std::cerr << "the file is write only" << std::endl;
                }
            }

            void write(float data) {
                if (m_mode == FileMode::WriteOnly) {
                    outfile << data << std::endl;
                } else {
                    std::cerr << "the file is write only" << std::endl;
                }
            }

            void close() {
                if (m_mode == FileMode::WriteOnly) {
                    outfile.close();
                }

                if (m_mode == FileMode::ReadOnly) {
                    infile.close();
                }
            }

            std::string readAll() {
                std::string data;
                if (m_mode == FileMode::ReadOnly) {
                    std::string buffer;
                    while (infile >> buffer) {
                        data += buffer + static_cast<char>(infile.peek());
                    }
                } else {
                    std::cerr << "the file is Read only" << std::endl;
                }
                return data;
            }

            template<class T>
            void read(T& data) {
                std::cout <<  "template" << std::endl;
                if (m_mode == FileMode::ReadOnly) {
                    infile.read(static_cast<T>(data), sizeof(data));
                } else {
                    std::cerr << "the file is read only" << std::endl;
                }
            }

            void read(std::string& data) {
                std::cout << "string" << std::endl;
                if (m_mode == FileMode::ReadOnly) {
                    std::string buffer;
                    while (infile >> buffer) {
                        data += buffer + static_cast<char>(infile.peek());
                    }
                } else {
                    std::cerr << "the file is read only" << std::endl;
                }
            }

            void read(int& data) {
                if (m_mode == FileMode::ReadOnly) {
                    infile >> data;
                } else {
                    std::cerr << "the file is read only" << std::endl;
                }
            }

            void read(float& data) {
                if (m_mode == FileMode::ReadOnly) {
                    infile >> data;
                } else {
                    std::cerr << "the file is read only" << std::endl;
                }
            }

            void remove() {
                if (m_mode == FileMode::WriteOnly) {
                    outfile << "" << std::endl;
                } else {
                    std::cerr << "the file is read only" << std::endl;
                }
            }
    };
}

#endif // CATCHCHALLENGER_FILE_H
