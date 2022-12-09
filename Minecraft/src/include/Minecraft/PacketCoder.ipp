#include <type_traits>
#include <numeric>
#include <functional>

#include "Minecraft/Utils/NetworkConversions.hpp"
#include "Minecraft/Utils/Bits.hpp"

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

template<Utils::Concepts::Integer T>
size_t PacketCoderImpl<T>::decode(DataSource& source, T& t){
    using uT = std::remove_cv_t<std::make_unsigned_t<T>>;

    uT uValue = 0;


    if constexpr(std::endian::native == std::endian::big){
        for (int offset = 0; offset < sizeof(uT) * 8; offset += 8){
            uValue |= static_cast<uT>(source.pullByte()) << offset;
        }
    }
    else{
        for (int offset = (sizeof(uT)-1) * 8; offset >= 0; offset -= 8){
            uValue |= static_cast<uT>(source.pullByte()) << offset;
        }
    }

    t = static_cast<T>(uValue);
    return sizeof(uT);
}
template<Utils::Concepts::Integer T>
size_t PacketCoderImpl<T>::encode(std::deque<uint8_t>& bytes, T const& t){
    using uT = std::remove_cv_t<std::make_unsigned_t<T>>;

    const uT uValue = static_cast<uT>(t);
    if constexpr(std::endian::native == std::endian::big){
        for (int offset = 0; offset < sizeof(T); offset += 8){
            bytes.push_back((uValue >> offset) & 0xFF);
        }
    }
    else{
        for (int offset = (sizeof(uT)-1) * 8; offset >= 0; offset -= 8){
            bytes.push_back((uValue >> offset) & 0xFF);
        }
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

template<std::floating_point T>
size_t PacketCoderImpl<T>::decode(DataSource& source, T& t){
    Utils::Integer<sizeof(T)*8> tmp;
    size_t bytesUsed = PacketCoder::decode(source, tmp);

    t = std::bit_cast<T>(tmp);

    return bytesUsed;
}
template<std::floating_point T>
size_t PacketCoderImpl<T>::encode(std::deque<uint8_t>& bytes, T const& t){
    return PacketCoder::encode(bytes, std::bit_cast<Utils::Integer<sizeof(T)*8>>(t));
}

size_t PacketCoderImpl<Position>::decode(DataSource& source, Position& t){
    uint64_t rawValue;
    size_t bytesUsed = PacketCoderImpl<uint64_t>::decode(source, rawValue);

    t.x = (rawValue >> 38) & Utils::binaryOnes<26>;
    t.z = (rawValue >> 12) & Utils::binaryOnes<26>;
    t.y = (rawValue >> 0 ) & Utils::binaryOnes<12>;

    if (t.x >= 1 << 25) t.x -= 1 << 26;
    if (t.y >= 1 << 11) t.y -= 1 << 12;
    if (t.z >= 1 << 25) t.z -= 1 << 26;

    return bytesUsed;
}
size_t PacketCoderImpl<Position>::encode(std::deque<uint8_t>& bytes, Position const& t){
    return PacketCoderImpl<uint64_t>::encode(bytes, ((t.x & Utils::binaryOnes<26>) << 38) | ((t.z & Utils::binaryOnes<26>) << 12) | (t.y & Utils::binaryOnes<12>));
}


template<CodableEnum T>
size_t PacketCoderImpl<T>::decode(DataSource& source, T& t){
    return PacketCoderImpl<EnumCodingType_t<T>>::decode(source, static_cast<EnumCodingType_t<T>>(t));
}
template<CodableEnum T>
size_t PacketCoderImpl<T>::encode(std::deque<uint8_t>& bytes, T const& t){
    return PacketCoderImpl<EnumCodingType_t<T>>::encode(bytes, static_cast<EnumCodingType_t<T>>(t));

}

}