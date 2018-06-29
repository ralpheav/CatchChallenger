#include <iostream>

#include <cstring>
#include <stdexcept>

class DataStreamSerializer {

    void* buffer;
    size_t last_size;

public:

    DataStreamSerializer(const unsigned int size = 1) {
        if (size == 0) {
            throw std::out_of_range("size must not set in zero");
        }
        last_size = 0;
        buffer = ::operator new(size);
    }

private:

    template <typename T> DataStreamSerializer& write(const T& value) {
        memcpy(buffer + last_size, &value, sizeof(T));
        last_size += sizeof(T);
        std::cout <<"size "<< last_size << std::endl;
    }

    DataStreamSerializer& write(const std::string& string) {
        size_t length = string.length();
        memcpy(buffer + last_size, &length, sizeof(int32_t));
        last_size += sizeof(int32_t);
        memcpy(buffer + last_size, string.c_str(), length);
        last_size += length;

        std::cout <<"size "<< last_size << std::endl;
    }

public:

    DataStreamSerializer& operator<<(const int8_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const int16_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const int32_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const uint8_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const uint16_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const uint32_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const std::string& string) {
        return write(string);
    }

    void *getSerialized() {
        return buffer;
    }

};

int main()
{
    int8_t x = 12;
    int32_t y = 51;
    std::string word = "this is my mesage for all the players...";
    DataStreamSerializer s_be(sizeof(x) + sizeof(y) + word.length() + 4);
    s_be << x;
    s_be << y;
    s_be << word;

    return 0;
}
