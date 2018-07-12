#include "File.h"

using namespace CatchChallenger;

File::File() {
    m_mode = ReadOnly;
}

File::File(const std::string& filename, FileMode mode = FileMode::WriteOnly) {
    m_filename = filename;
    open(mode);
}

bool File::open(FileMode mode) {
    m_mode = mode;
    ios_base::openmode iosMode;
    if (m_mode == FileMode::WriteOnly) {
        iosMode = ios::out;
    }
    if (m_mode == FileMode::ReadOnly) {
        iosMode = ios::in;
    }

    m_file.open(m_filename, ios::in);

    return m_file.is_open();
}

bool File::isOpen() {
    return m_file.is_open();
}

FileMode File::mode() {
    return m_mode;
}

void File::setFileName(const std::string& filename) {
    m_filename = filename;
}

std::string File::filename() const {
    return m_filename;
}

bool File::exists() {
    std::ifstream auxFile(m_filename);
    return auxFile.good();
}

void File::write(const std::string& data) {
    if (m_mode == FileMode::WriteOnly) {
        m_file << data << std::endl;
    } else {
        std::cerr << "the file is write only" << std::endl;
    }
}

template<class T>
void File::write(T data) {
    if (m_mode == FileMode::WriteOnly) {
        m_file.write(static_cast<T>(data), sizeof(data));
    } else {
        std::cerr << "the file is write only" << std::endl;
    }

}

void File::write(int data) {
    if (m_mode == FileMode::WriteOnly) {
        m_file << data << std::endl;
    } else {
        std::cerr << "the file is write only" << std::endl;
    }
}

void File::write(float data) {
    if (m_mode == FileMode::WriteOnly) {
        m_file << data << std::endl;
    } else {
        std::cerr << "the file is write only" << std::endl;
    }
}

void File::close() {
     m_file.close();
}

std::string File::readAll() {
    std::string data;
    if (m_mode == FileMode::ReadOnly) {
        std::string buffer;
        while (m_file >> buffer) {
            data += buffer + static_cast<char>(infile.peek());
        }
    } else {
        std::cerr << "the file is Read only" << std::endl;
    }
    return data;
}

template<class T>
void File::read(T& data) {
    std::cout <<  "template" << std::endl;
    if (m_mode == FileMode::ReadOnly) {
        m_file.read(static_cast<T>(data), sizeof(data));
    } else {
        std::cerr << "the file is read only" << std::endl;
    }
}

void File::read(std::string& data) {
    std::cout << "string" << std::endl;
    if (m_mode == FileMode::ReadOnly) {
        std::string buffer;
        while (m_file >> buffer) {
            data += buffer + static_cast<char>(m_file.peek());
        }
    } else {
        std::cerr << "the file is read only" << std::endl;
    }
}

void File::read(int& data) {
    if (m_mode == FileMode::ReadOnly) {
        m_file >> data;
    } else {
        std::cerr << "the file is read only" << std::endl;
    }
}

void File::read(float& data) {
    if (m_mode == FileMode::ReadOnly) {
        m_file >> data;
    } else {
        std::cerr << "the file is read only" << std::endl;
    }
}

bool File::remove() {
    if (m_mode == FileMode::WriteOnly) {
        m_file.flush();
        m_file << "" << std::endl;
        return true;
    } else {
        std::cerr << "the file is read only" << std::endl;
    }
    return false;
}

bool File::deleteFile() {
    std::remove(m_filename);
    bool deleted = std::ifstream::open(m_filename);
    if  (!deleted) {
        std::cerr << "The file cannot be deleted." << std::endl;
    }
    return deleted;
}

std::string File::errorString() {
    //m_lastError = stderr(errno);
    //redir the cerr
    return m_lastError;
}
