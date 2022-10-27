#pragma once

#include "Minecraft/PacketID.hpp"

#include <deque>
#include <vector>
#include <string>
#include <condition_variable>
#include <mutex>

#include <stdint.h>
#include <stddef.h>

namespace Minecraft{

/**
 * @brief An encoder and decoder for Minecraft packets
 * 
 * Allows for simultaneous encoding and decoding by one thread each.
 */
class PacketCoder{
    public:
        /**
         * @brief Construct an empty Packet Data object
         * 
         */
        PacketCoder();

        /**
         * @brief Construct a new Packet Coder object from bytes
         * 
         * @param data the bytes
         */
        PacketCoder(std::vector<uint8_t> const& data);

        /**
         * @brief Get the encoded bytes
         * 
         * @return std::vector<uint8_t> the bytes
         */
        std::vector<uint8_t> getBytes() const;

        /**
         * @brief Get a number of encoded bytes
         * 
         * This operation will block until the given
         * number of bytes is available.
         * 
         * @param numBytes the number of bytes to return
         * @return std::vector<uint8_t> the bytes
         */
        std::vector<uint8_t> getBytes(size_t numBytes) const;

        /**
         * @brief Convert to a complete packet
         * 
         * @param id the packet id
         * @return std::vector<uint8_t> bytes of the full packet
         */
        std::vector<uint8_t> toPacket(PacketID id) const;

        /**
         * @brief Get the number of decoded bytes
         * 
         * @return size_t number of bytes decoded since last calling this function
         */
        size_t getNumDecodedBytes();

        /**
         * @brief Ignore some bytes while decoding
         * 
         * @param numBytes number of bytes to ignore
         */
        void ignore(size_t numBytes);

        /**
         * @brief Get number of encoded bytes
         * 
         * @return size_t number of bytes in this instance
         */
        size_t size() const;

        /**
         * @brief Encode a boolean
         * 
         * @param value the boolean
         * @return PacketCoder& this instance
         */
        PacketCoder& operator<< (bool value);

        /**
         * @brief Encode a var int
         * 
         * Converts from an int32_t to a Minecraft var int.
         * 
         * @param value the integer
         * @return PacketCoder& this instance
         */
        PacketCoder& operator<< (int32_t value);

        /**
         * @brief Encode a string
         * 
         * @param value the string
         * @return PacketCoder& this instance
         */
        PacketCoder& operator<< (std::string value);

        /**
         * @brief Encode an unsigned short.
         * 
         * Converts to the correct endiannes.
         * 
         * @param value the ushort
         * @return PacketCoder& this instance
         */
        PacketCoder& operator<< (uint16_t value);

        /**
         * @brief Insert raw bytes
         * 
         * @param rawBytes bytes to insert
         * @return PacketCoder& this instance
         */
        PacketCoder& operator<< (std::vector<uint8_t> const& rawBytes);

        /**
         * @brief Insert raw bytes
         * 
         * @param rawBytes bytes to insert
         * @return PacketCoder& this instance
         */
        PacketCoder& operator<< (std::deque<uint8_t> const& rawBytes);



        /**
         * @brief Decode a boolean
         * 
         * @param value the boolean
         * @return PacketCoder& this instance
         */
        PacketCoder& operator>> (bool& value);

        /**
         * @brief Decode a var int
         * 
         * Converts from an int32_t to a Minecraft var int.
         * 
         * @param value the integer
         * @return PacketCoder& this instance
         */
        PacketCoder& operator>> (int32_t& value);

        /**
         * @brief Decode a string
         * 
         * @param value the string
         * @return PacketCoder& this instance
         */

        PacketCoder& operator>> (std::string& value);

        /**
         * @brief Decode an unsigned short.
         * 
         * Converts to the correct endiannes.
         * 
         * @param value the ushort
         * @return PacketCoder& this instance
         */
        PacketCoder& operator>> (uint16_t& value);

        

    private:
        std::deque<uint8_t> bytes;
        size_t numDecodedBytes = 0;

        mutable std::mutex mutex;
        mutable std::condition_variable condition;
};

}