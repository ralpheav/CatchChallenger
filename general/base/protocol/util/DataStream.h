#include <vector>
#include <memory>
#include <cstdint>
#include <cassert>

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

    enum Flag {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

private:
    std::shared_ptr<const Storage> m_ptr;
    const Storage* m_data;
    size_t m_idx;
    Status m_status;
    ByteOrder m_byteOrder;
    ByteOrder m_systemOrder;
    Flag m_mode;

    static ByteOrder systemByteOrder() {
        const uint32_t t = 1;
        return (reinterpret_cast<const uint8_t*>(&t)) ? LittleEndian : BigEndian;
    }

    bool has(size_t count) const {
        return m_idx + count <= m_data->size();
    }

    template <typename T> DataStream& read(T& i) {
    }

    template <typename T> DataStream& write(const T& i) {
    }


public:
    DataStream(const std::vector<uint8_t>* data, Flag mode) :
        m_data(data),
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
