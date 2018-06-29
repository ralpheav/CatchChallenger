#include <iostream>

#include <vector>
#include <memory>
#include <cstdint>
#include <cassert>
#include <cstring>
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

    struct Buffer {
        void* data;
        Buffer* next;
    };

    DataStream() {
        pBuffer = nullptr;
        buffer = nullptr;
        pIndex = nullptr;
    }

private:
    std::shared_ptr< Storage> m_ptr;
    Storage* m_data;
    size_t m_idx;
    Status m_status;
    ByteOrder m_byteOrder;
    ByteOrder m_systemOrder;
    AccessMode m_mode;

    Buffer* pBuffer;
    Buffer* buffer;
    Buffer* pIndex;

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

    template <typename T> DataStream& writeMem(const T& value) {
        pBuffer = new Buffer();
        if (!buffer) {
            buffer = pBuffer;
        } else {
            pIndex->next = pBuffer;
        }
        //populating
        pBuffer->data = malloc(sizeof(T));
        memcpy(pBuffer->data, &value, sizeof(T));
        pBuffer->next = nullptr;
        //update
        pIndex = pBuffer;
    }
public:
    void viewBuffer() const{
        Buffer* p = buffer;
        while(p) {
            std::cout << "buffer: " << p->data << std::endl;
            p = p->next;
        }
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
        return writeMem(i);
    }

    DataStream& operator<<(const int16_t& i) {
        return writeMem(i);
    }

    DataStream& operator<<(const int32_t& i) {
        return writeMem(i);
    }

    DataStream& operator<<(const uint8_t& i) {
        return writeMem(i);
    }

    DataStream& operator<<(const uint16_t& i) {
        return writeMem(i);
    }

    DataStream& operator<<(const uint32_t& i) {
        return writeMem(i);
    }

    void setByteOrder(ByteOrder b) {
        m_byteOrder = b;
    }

    ByteOrder byteOrder() const {
        return m_byteOrder;
    }
};



int main()
{
    std::vector<uint8_t> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    uint32_t val;

    DataStream s_be(&v, DataStream::ReadOnly);
    s_be >> val;
    assert(val == 0x01020304); // big endian
    std::cout <<"pri "<< val << std::endl;
    s_be << 48;
    s_be << 1;
    s_be << 58;
    s_be.viewBuffer();

    DataStream s_le(&v, DataStream::ReadOnly);
    s_le.setByteOrder(DataStream::LittleEndian);
    s_le >> val;
    assert(val == 0x04030201); // little endian
    std::cout <<"sec "<< val << std::endl;

    uint32_t val1;
    ByteArray dd;
    dd.push_back(6);
    dd.push_back(4);
    dd.push_back(2);
    DataStream forByte(dd, DataStream::ReadOnly);
    forByte << 7;
    forByte << 8;
    forByte >> val1;
    forByte >> val1;

    std::cout <<"ter "<< val1 << std::endl;

    return 0;
}
