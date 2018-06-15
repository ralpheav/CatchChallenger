#include <vector>
#include <memory>
#include <cstdint>
#include <cassert>
#include "ByteArray.h"

class DataStream {

public:
    typedef std::vector<uint8_t> Storage; //std::vector<unsigned char>

    enum ByteOrder {
        BigEndian,
        LittleEndian
    };

    enum Status {
        Ok,
        ReadPastEnd
    };

    enum AccessMode {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

private:
    std::shared_ptr< Storage> m_ptr;
    Storage* m_data;
    size_t m_idx;
    Status m_status;
    ByteOrder m_byteOrder;
    ByteOrder m_systemOrder;
    AccessMode m_mode;

    static ByteOrder systemByteOrder() {
        const uint32_t t = 1;

        return (reinterpret_cast<const uint8_t*>(&t)) ? LittleEndian : BigEndian;
    }

    bool has(size_t count) const {

        return m_idx + count <= m_data->size();
    }

    template <typename T> DataStream& read(T& i) {
        if (has(sizeof(T)) && Ok == m_status) {
            T result = *reinterpret_cast<const T*>(&(*m_data)[m_idx]);
            m_idx += sizeof(T);
            if (m_byteOrder != m_systemOrder) {
                T tmp = 0;
                for (uint8_t index = 0; index < sizeof(T); ++index) {
                    tmp = (tmp << 8) | (result & 0xFF);
                    result = result >> 8;
                }
                i = tmp;
            } else
                i = result;
        } else {
            m_status = ReadPastEnd;
        }
        return *this;
    }

    template <typename T> DataStream& write(const T& i) {
        if (m_data && Ok == m_status) {
            m_data->push_back(i);
            m_idx++;
        } else {
            m_status = ReadPastEnd;
        }
        return *this;
    }

public:

    DataStream(std::vector<uint8_t>* data, AccessMode mode) :
        m_data(data),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()),
        m_mode(mode) {

    }

    DataStream(std::shared_ptr<Storage> data, AccessMode mode) :
        m_ptr(data),
        m_data(m_ptr.get()),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()),
        m_mode(mode) {

    }

    DataStream(std::unique_ptr<Storage> && data, AccessMode mode) :
        m_ptr(data.release()),
        m_data(m_ptr.get()),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()),
        m_mode(mode) {

    }

    DataStream(Storage && data, AccessMode mode) :
        m_ptr(new Storage(std::move(data))),
        m_data(m_ptr.get()),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()),
        m_mode(mode) {

    }

    DataStream(ByteArray data, AccessMode mode) :
        m_data(new Storage(data.get())),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()),
        m_mode(mode) {
    }

    bool atEnd() const {
        return m_idx == m_data->size();
    }

    DataStream& operator>>(int8_t& i) {
        return read(i);
    }

    DataStream& operator>>(int16_t& i) {
        return read(i);
    }

    DataStream& operator>>(int32_t& i) {
        return read(i);
    }

    DataStream& operator>>(uint8_t& i) {
        return read(i);
    }

    DataStream& operator>>(uint16_t& i) {
        return read(i);
    }

    DataStream& operator>>(uint32_t& i) {
        return read(i);
    }

    DataStream& operator<<(const int8_t& i) {
        return write(i);
    }

    DataStream& operator<<(const int16_t& i) {
        return write(i);
    }

    DataStream& operator<<(const int32_t& i) {
        return write(i);
    }

    DataStream& operator<<(const uint8_t& i) {
        return write(i);
    }

    DataStream& operator<<(const uint16_t& i) {
        return write(i);
    }

    DataStream& operator<<(const uint32_t& i) {
        return write(i);
    }

    void setByteOrder(ByteOrder b) {
        m_byteOrder = b;
    }

    ByteOrder byteOrder() const {
        return m_byteOrder;
    }
};
