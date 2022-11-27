#include <type_traits>
#include <numeric>
#include <functional>

namespace Minecraft{

template<Codable T>
size_t PacketCoderImpl<T>::decode(DataSource& source, T& t){
    return std::apply([&source](auto&... contents) -> size_t {
            return (PacketCoder::decode(source, contents) + ...);
        }, t.contents());
}
template<Codable T>
size_t PacketCoderImpl<T>::encode(std::deque<uint8_t>& bytes, T const& t){
    return std::apply([&bytes](auto const&... contents) -> size_t {
            return (PacketCoder::encode(bytes, contents) + ...);
        }, t.contents());
}

size_t PacketCoderImpl<bool>::decode(DataSource& source, bool& t){
    if (source.readByte() == 0x00)
        t = false;
    else if (source.readByte() == 0x01)
        t = true;
    else{
        std::stringstream ss;
        ss << "Invalid value 0x"
           << std::hex << std::setw(2) << std::setfill('0')
           << source.pullByte()
           << std::dec << std::setw(0)
           << "for boolean.";
        throw std::runtime_error(ss.str());
    }
    source.pullByte();
    return 1;
}
size_t PacketCoderImpl<bool>::encode(std::deque<uint8_t>& bytes, bool const& t){
    bytes.push_back(t? 0x01 : 0x00);
    return 1;
}

size_t PacketCoderImpl<std::string>::decode(DataSource& source, std::string& t){
    VarInt size;
    size_t usedBytes = PacketCoderImpl<VarInt>::decode(source, size);
    usedBytes += size;

    auto stringContent = source.pullBytes(size);

    t.clear();
    t.insert(t.begin(), stringContent.begin(), stringContent.end());
    return usedBytes;
}
size_t PacketCoderImpl<std::string>::encode(std::deque<uint8_t>& bytes, std::string const& t){
    size_t usedBytes = PacketCoderImpl<VarInt>::encode(bytes, VarInt(t.size()));
    bytes.insert(bytes.end(), t.begin(), t.end());
    
    return usedBytes + t.size();
}

template<uint SIZE>
size_t PacketCoderImpl<VarInteger<SIZE>>::decode(DataSource& source, VarInteger<SIZE>& t){
    using T = typename VarInteger<SIZE>::type;
    using uT = std::make_unsigned_t<T>;

    size_t usedBytes = 0;

    uT uValue = 0;
    int shiftAmount=0;
    uint8_t byte;
    do{
        if (shiftAmount >= SIZE){
            throw std::runtime_error("Variable integer to big!");
        }
        byte = source.pullByte();
        usedBytes++;

        uValue |= static_cast<uT>((byte & DATA_BITS)) << shiftAmount;
        shiftAmount += 7;

    } while (byte & CONTINUE_BIT);
    t.value = static_cast<T>(uValue);

    return usedBytes;
}
template<uint SIZE>
size_t PacketCoderImpl<VarInteger<SIZE>>::encode(std::deque<uint8_t>& bytes, VarInteger<SIZE> const& t){
    using T = typename VarInteger<SIZE>::type;
    using uT = std::make_unsigned_t<T>;

    size_t usedBytes = 0;

    uT uValue = static_cast<uT>(t.value);

    do {
        auto& byte = bytes.emplace_back();
        usedBytes++;

        byte = uValue & DATA_BITS;
        uValue &= (uT)~DATA_BITS;

        byte |= (uValue? 1 : 0) << 7;
        uValue >>= 7;
    } while (uValue);

    return usedBytes;
}

template<Utils::Concepts::integer T>
size_t PacketCoderImpl<T>::decode(DataSource& source, T& t){
    using uT = std::remove_cv_t<std::make_unsigned_t<T>>;

    uT uValue = 0;

    for (int offset = (sizeof(uT)-1) * 8; offset >= 0; offset -= 8){
        uValue |= static_cast<uT>(source.pullByte()) << offset;
    }

    t = static_cast<T>(uValue);
    return sizeof(uT);
}
template<Utils::Concepts::integer T>
size_t PacketCoderImpl<T>::encode(std::deque<uint8_t>& bytes, T const& t){
    using uT = std::remove_cv_t<std::make_unsigned_t<T>>;

    const uT uValue = static_cast<uT>(t);

    for (int offset = (sizeof(uT)-1) * 8; offset >= 0; offset -= 8){
        bytes.push_back((uValue >> offset) & 0xFF);
    }
    return sizeof(uT);
}

template<typename T>
size_t PacketCoderImpl<Conditional<T>>::decode(DataSource& source, Conditional<T>& t){
    // this works because the condition will already be decoded
    if (t.isPresent())
        return PacketCoderImpl<T>::decode(source, t.value);
    else
        return 0;
}
template<typename T>
size_t PacketCoderImpl<Conditional<T>>::encode(std::deque<uint8_t>& bytes, Conditional<T> const& t){
    if (t.isPresent())
        return PacketCoderImpl<T>::encode(bytes, t.value);
    else
        return 0;
}


template<typename T>
size_t PacketCoderImpl<std::vector<T>>::decode(DataSource& source, std::vector<T>& t){
    return std::accumulate(t.begin(), t.end(), 0, [&source](size_t bytesUsed, auto& x) -> size_t {
            return bytesUsed + PacketCoderImpl<T>::decode(source, x);
        });
}
template<typename T>
size_t PacketCoderImpl<std::vector<T>>::encode(std::deque<uint8_t>& bytes, std::vector<T> const& t){
    return std::accumulate(t.begin(), t.end(), 0, [&bytes](size_t bytesUsed, auto const& x) -> size_t {
            return bytesUsed + PacketCoderImpl<T>::encode(bytes, x);
        });
}

}