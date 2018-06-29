#include <vector>
#include <memory>
#include <cstdint>
#include <cassert>

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

private:
    std::shared_ptr<const Storage> m_ptr;
    const Storage* m_data;
    size_t m_idx;
    Status m_status;
    ByteOrder m_byteOrder;
    ByteOrder m_systemOrder;

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

public:
    DataStream(const std::vector<uint8_t>* data) :
        m_data(data),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()) {

    }
    DataStream(std::shared_ptr<Storage> data) :
        m_ptr(data),
        m_data(m_ptr.get()),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()) {

    }
    DataStream(std::unique_ptr<Storage> && data) :
        m_ptr(data.release()),
        m_data(m_ptr.get()),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()) {

    }
    DataStream(Storage && data) :
        m_ptr(new Storage(std::move(data))),
        m_data(m_ptr.get()),
        m_idx(0),
        m_status(Ok),
        m_byteOrder(BigEndian),
        m_systemOrder(systemByteOrder()) {

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
    DataStream s_be(&v);
    s_be >> val;
    assert(val == 0x01020304); // big endian
    DataStream s_le(&v);
    s_le.setByteOrder(DataStream::LittleEndian);
    s_le >> val;
    assert(val == 0x04030201); // little endian
    return 0;
}
