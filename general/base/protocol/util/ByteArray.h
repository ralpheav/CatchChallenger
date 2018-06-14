#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include <vector>

class ByteArray {
    std::vector<unsigned char> storage;
public:
    const char* constData() {
        return storage.data();
    }
    size_t size() {
       return storage.size();
    }
};

#endif //BYTE_ARRAY_H
