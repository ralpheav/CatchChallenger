#ifndef DATASTREAMSERIALIZER_H
#define DATASTREAMSERIALIZER_H

#include <cstring>
#include <stdexcept>

/**
 * @brief The DataStreamSerializer class
 */
class DataStreamSerializer {

    void* buffer;
    size_t last_size;
    uint32_t position;
    bool open;

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
        position = 0;
        buffer = ::operator new(size);
        open = false;
    }

    /**
     * @brief DataStreamSerializer
     */
    DataStreamSerializer(bool openToRead = false) {
        open = openToRead;
        position = sizeof(buffer);
    }

    /**
     * @brief isOpen
     * @return
     */
    bool isOpen() {
        return open;
    }

    /**
     * @brief setOpen
     * @param open
     */
    void setOpen(bool open = true) {
        this->open = open;
    }

private:

    /**
     * Write template for int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t
     * @brief write
     * @param T
     * @return
     */
    template <typename T> DataStreamSerializer& write(const T& value) {
        memcpy(((uint8_t*)buffer) + last_size, &value, sizeof(T));
        last_size += sizeof(T);
        std::cout << "size "<< last_size << std::endl;
	position = last_size - 1;
    }

    /**
     * Reading template for int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t
     * @brief read
     * @param T
     * @return
     */
    template <typename T> DataStreamSerializer& read(T& out_value) {
        if (open) {
            memcpy(&out_value, buffer, sizeof(T));
            buffer = ((uint8_t*)buffer) + sizeof(T);
            position -= sizeof(T);
        }
    }

    /**
     * Write for strings
     * @brief write
     * @param string
     * @return
     */
    DataStreamSerializer& write(const std::string& in_string) {
        size_t length = in_string.length();
        memcpy(((uint8_t*)buffer) + last_size, &length, sizeof(int8_t));//TODO: should it be int8_t ??? or more
        last_size += sizeof(int8_t);
        memcpy(((uint8_t*)buffer) + last_size, in_string.c_str(), length);
        last_size += length;

        std::cout << "size "<< last_size << std::endl;
	position = last_size - 1;

        return *this;
    }

    /**
     * @brief read
     * @param string
     * @return
     */
    DataStreamSerializer& read(std::string& out_string) {
        if (open) {
            size_t length;
            memcpy(&length, buffer, sizeof(int8_t));
            buffer = ((uint8_t*)buffer) + sizeof(int8_t);
            position -= sizeof(int8_t);

            char deserializated_string[length];
            memcpy(deserializated_string, &buffer, length);
            buffer = ((uint8_t*)buffer) + length;
            position -= length;

            out_string = deserializated_string;
        }
        return *this;
    }

public:

    //Write into bytes stream

    DataStreamSerializer& operator<<(const int8_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const int16_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const int32_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const int64_t& i) {
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

    DataStreamSerializer& operator<<(const uint64_t& i) {
        return write(i);
    }

    DataStreamSerializer& operator<<(const std::string& out_string) {
        return write(out_string);
    }

    //Reading byte stream

    DataStreamSerializer& operator>>(int8_t& i) {
        this->read(i);
        return *this;
    }

    DataStreamSerializer& operator>>(int16_t& i) {
        this->read(i);
        return *this;
    }

    DataStreamSerializer& operator>>(int32_t& i) {
        this->read(i);
        return *this;
    }

    DataStreamSerializer& operator>>(int64_t& i) {
        this->read(i);
        return *this;
    }

    DataStreamSerializer& operator>>(uint8_t& i) {
        this->read(i);
        return *this;
    }

    DataStreamSerializer& operator>>(uint16_t& i) {
        this->read(i);
        return *this;
    }

    DataStreamSerializer& operator>>(uint32_t& i) {
        this->read(i);
        return *this;
    }

    DataStreamSerializer& operator>>(uint64_t& i) {
        this->read(i);
        return *this;
    }

    DataStreamSerializer& operator>>(std::string& in_string) {
        this->read(in_string);
        return *this;
    }

    /**
     * @brief data
     * @return
     */
    void *data() {
        return buffer;
    }

    /**
     * @brief constData
     * @return
     */
    const void *constData() {
        return buffer;
    }

    /**
     * @brief size
     * @return
     */
    size_t size() {
        return last_size;
    }

    /**
     * @brief pos
     * @return
     */
    uint32_t pos() {
        return position;
    }

};

#endif //DATASTREAMSERIALIZER_H
