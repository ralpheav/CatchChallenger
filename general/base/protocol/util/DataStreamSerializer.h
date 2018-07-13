#ifndef CATCHCHALLENGER_DATASTREAMSERIALIZER_H
#define CATCHCHALLENGER_DATASTREAMSERIALIZER_H

#include <iostream>
#include <cstring>
#include <stdexcept>

namespace CatchChallenger
{

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
        DataStreamSerializer(const unsigned int size);

        /**
         * @brief DataStreamSerializer
         */
        DataStreamSerializer(bool openToRead);

        /**
         * @brief isOpen
         * @return
         */
        bool isOpen();

        /**
         * @brief setOpen
         * @param open
         */
        void setOpen(bool open);

    private:

        /**
         * Write template for int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t
         * @brief write
         * @param T
         * @return
         */
        template <typename T> DataStreamSerializer& write(const T& value);

        /**
         * Reading template for int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t
         * @brief read
         * @param T
         * @return
         */
        template <typename T> DataStreamSerializer& read(T& out_value);

        /**
         * Write for strings
         * @brief write
         * @param string
         * @return
         */
        DataStreamSerializer& write(const std::string& in_string);

        /**
         * @brief read
         * @param string
         * @return
         */
        DataStreamSerializer& read(std::string& out_string);

    public:

        //Write into bytes stream

        DataStreamSerializer& operator<<(const int8_t& i);
        DataStreamSerializer& operator<<(const int16_t& i);
        DataStreamSerializer& operator<<(const int32_t& i);
        DataStreamSerializer& operator<<(const int64_t& i);
        DataStreamSerializer& operator<<(const uint8_t& i);
        DataStreamSerializer& operator<<(const uint16_t& i);
        DataStreamSerializer& operator<<(const uint32_t& i);
        DataStreamSerializer& operator<<(const uint64_t& i);
        DataStreamSerializer& operator<<(const std::string& out_string);

        //Reading byte stream

        DataStreamSerializer& operator>>(int8_t& i);
        DataStreamSerializer& operator>>(int16_t& i);
        DataStreamSerializer& operator>>(int32_t& i);
        DataStreamSerializer& operator>>(int64_t& i);
        DataStreamSerializer& operator>>(uint8_t& i);
        DataStreamSerializer& operator>>(uint16_t& i);
        DataStreamSerializer& operator>>(uint32_t& i);
        DataStreamSerializer& operator>>(uint64_t& i);
        DataStreamSerializer& operator>>(std::string& in_string);

        /**
         * @brief data
         * @return
         */
        void *data();

        /**
         * @brief constData
         * @return
         */
        const void *constData();

        /**
         * @brief size
         * @return
         */
        size_t size();

        /**
         * @brief pos
         * @return
         */
        uint32_t pos();

    };
}

#endif //CATCHCHALLENGER_DATASTREAMSERIALIZER_H
