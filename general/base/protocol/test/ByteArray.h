#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include <vector>

class ByteArray {
    std::vector<uint8_t> storage;
public:
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
};

#endif //BYTE_ARRAY_H
