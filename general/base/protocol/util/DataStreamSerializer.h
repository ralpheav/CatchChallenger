#include <cstring>
#include <stdexcept>

/**
 * @brief The DataStreamSerializer class
 */
class DataStreamSerializer {

    void* buffer;
    size_t last_size;

public:

    /**
     * @brief DataStreamSerializer
     * @param size
     */
    DataStreamSerializer(const unsigned int size = 1) {
        if (size == 0) {
            throw std::out_of_range("Size must not set with zero");
        }
        last_size = 0;
        buffer = ::operator new(size);
    }

private:

    /**
     * Write template for int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t
     * @brief write
     * @param string
     * @return
     */
    template <typename T> DataStreamSerializer& write(const T& value) {
        memcpy(buffer + last_size, &value, sizeof(T));
        last_size += sizeof(T);
        std::cout <<"size "<< last_size << std::endl;
    }

    /**
     * Write for strings
     * @brief write
     * @param string
     * @return
     */
    DataStreamSerializer& write(const std::string& string) {
        size_t length = string.length();
        memcpy(buffer + last_size, &length, sizeof(int8_t));//TODO: should it be int8_t ??? or more
        last_size += sizeof(int8_t);
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

    void *data() {
        return buffer;
    }

    const void *constData() {
        return buffer;
    }

    size_t size() {
        return last_size;
    }
};
