#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include <vector>
#include <stdexcept>

class ByteArray {
    std::vector<uint8_t> storage;
public:
    ByteArray () {}
//    ByteArray(uint8_t* source, size_t size) {
//        int index = 0;
//        while (index < size) {
//            storage.push_back(*(source + index));
//            index++;
//        }
//    }

    ByteArray(unsigned char* source, size_t size) {
        int index = 0;
        uint8_t* sourceCopy = static_cast<uint8_t*>(source);
        while (index < size) {
            storage.push_back(*(sourceCopy + index));
            index++;
        }
    }

    ByteArray(const char* source, size_t size) {
        int index = 0;
        uint8_t* sourceCopy = (uint8_t*)source;
        while (index < size) {
            storage.push_back(*(sourceCopy + index));
            index++;
        }
    }

    const char* constData() const {
        return (char *)(storage.data());
    }

    char* data() const {
        return (char *)(storage.data());
    }

    size_t size() const {
       return storage.size();
    }

    void push_back(uint8_t byte) {
        storage.push_back(byte);
    }

    std::vector<uint8_t> get() {
        return storage;
    }

    std::vector<uint8_t>* ptrget() {
        return &storage;
    }

    const char* toHex() const {
        return (char *)(storage.data());
    }
};

#endif //BYTE_ARRAY_H
