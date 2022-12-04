#pragma once

#include "Minecraft/PacketID.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Conditional.hpp"
#include "Minecraft/DataSink.hpp"
#include "Minecraft/DataSource.hpp"
#include "Minecraft/Position.hpp"
#include "Minecraft/UUID.hpp"
#include "Minecraft/Codable.hpp"

#include "Minecraft/Utils/Concepts.hpp"
#include "Minecraft/Utils/Bits.hpp"

#include <deque>
#include <string>
#include <concepts>
#include <vector>

#include <stdint.h>
#include <stddef.h>

namespace Minecraft{

/**
 * 
 * @brief Encodes/Decodes objects to/from bytes
 * 
 * @tparam T the type of object
 */
template<typename T>
struct PacketCoderImpl;


/**
 * @brief An encoder and decoder for Minecraft packets.
 */
class PacketCoder{
    public:
        /**
         * @brief Encode an object as bytes.
         * 
         * @tparam T the type of object
         * @param bytes the bytes to encode to
         * @param t the object to encode
         * @return size_t the number of bytes used to encode the object
         */
        template<typename T>
        static size_t encode(std::deque<uint8_t>& bytes, T const& t){
            return PacketCoderImpl<T>::encode(bytes, t);
        }

        /**
         * @brief Encode a packet and push it into the sink.
         * 
         * @tparam T the type holding the packet contentd
         * @param sink the sink the packet gets pushed to
         * @param id the packet id
         * @param t the packet data
         * @return size_t the number of bytes used to encode the object
         */
        template<typename T>
        static size_t encodePacket(DataSink& sink, PacketID id, T const& t){
            size_t usedBytes = 0;

            Minecraft::VarInt packetID = static_cast<int32_t>(id);
            Minecraft::VarInt dataSize;

            std::deque<uint8_t> data;
            usedBytes += PacketCoder::encode(data, t);

            {
                std::deque<uint8_t> tmp;
                usedBytes += PacketCoder::encode(tmp, packetID);
            }
            dataSize = usedBytes;

            std::deque<uint8_t> header;
            usedBytes += PacketCoder::encode(header, dataSize);
            PacketCoder::encode(header, packetID);

            // join header and data
            data.insert(data.begin(), header.begin(), header.end());

            sink.pushBytes(data);
            return usedBytes;
        }

        /**
         * @brief Decode an object from bytes.
         * 
         * @tparam T the type of object
         * @param bytes the bytes to decode from
         * @param t the object to decode into
         * @return size_t the number of bytes used to decode the object
         */
        template<typename T>
        static size_t decode(DataSource& source, T& t){
            return PacketCoderImpl<T>::decode(source, t);
        }
};


template<Codable T>
struct PacketCoderImpl<T>{
    static size_t decode(DataSource& source, T& t);
    static size_t encode(std::deque<uint8_t>& bytes, T const& t);
};
template<>
struct PacketCoderImpl<bool>{
    static size_t decode(DataSource& source, bool& t);
    static size_t encode(std::deque<uint8_t>& bytes, bool const& t);
};

template<>
struct PacketCoderImpl<std::string>{
    static size_t decode(DataSource& source, std::string& t);
    static size_t encode(std::deque<uint8_t>& bytes, std::string const& t);
};

template<uint SIZE>
struct PacketCoderImpl<VarInteger<SIZE>>{
    static size_t decode(DataSource& source, VarInteger<SIZE>& t);
    static size_t encode(std::deque<uint8_t>& bytes, VarInteger<SIZE> const& t);

    static constexpr uint8_t DATA_BITS = Minecraft::Utils::binaryOnes<7>;
    static constexpr uint8_t CONTINUE_BIT = ~DATA_BITS;

};

template<Utils::Concepts::integer T>
struct PacketCoderImpl<T>{
    static size_t decode(DataSource& source, T& t);
    static size_t encode(std::deque<uint8_t>& bytes, T const& t);
};

template<typename T>
struct PacketCoderImpl<Conditional<T>>{
    static size_t decode(DataSource& source, Conditional<T>& t);
    static size_t encode(std::deque<uint8_t>& bytes, Conditional<T> const& t);
};

template<typename T>
struct PacketCoderImpl<std::vector<T>>{
    static size_t decode(DataSource& source, std::vector<T>& t);
    static size_t encode(std::deque<uint8_t>& bytes, std::vector<T> const& t);
};

template<std::floating_point T>
struct PacketCoderImpl<T>{
    static size_t decode(DataSource& source, T& t);
    static size_t encode(std::deque<uint8_t>& bytes, T const& t);
};

template<>
struct PacketCoderImpl<Position>{
    static size_t decode(DataSource& source, Position& t);
    static size_t encode(std::deque<uint8_t>& bytes, Position const& t);
};

}

#include "Minecraft/PacketCoder.ipp"