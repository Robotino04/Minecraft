#pragma once

#include "Minecraft/PacketID.hpp"

#include <vector>
#include <string>

#include <stdint.h>
#include <stddef.h>

namespace Minecraft{

/**
 * @brief Some Minecraft packet data
 * 
 * A wrapper for a std::vector<uint8_t> with
 * stream operators to convert to and from 
 * the Minecraft format.
 */
class PacketData{
    public:
        /**
         * @brief Construct an empty Packet Data object
         * 
         */
        PacketData();

        /**
         * @brief Construct a new Packet Data object
         * 
         * @param size default size for the wrapped vector
         */
        PacketData(size_t size);

        /**
         * @brief Construct a new Packet Data object from bytes
         * 
         * @param data the bytes
         */
        PacketData(std::vector<uint8_t> const& data);

        /**
         * @brief Get the bytes
         * 
         * @return std::vector<uint8_t> the bytes
         */
        std::vector<uint8_t> getBytes() const;

        /**
         * @brief Convert to a complete packet
         * 
         * @param id the packet id
         * @return PacketData the full packet
         */
        PacketData toPacket(PacketID id) const;

        
        /**
         * @brief Insert a boolean
         * 
         * @param value the boolean
         * @return PacketData& this instance
         */
        PacketData& operator<< (bool value);

        /**
         * @brief Insert a var int
         * 
         * Converts from an int32_t to a Minecraft var int.
         * 
         * @param value the integer
         * @return PacketData& this instance
         */
        PacketData& operator<< (int32_t value);

        /**
         * @brief Insert a string
         * 
         * @param value the string
         * @return PacketData& this instance
         */
        PacketData& operator<< (std::string value);

        /**
         * @brief Insert an unsigned short.
         * 
         * Converts to the correct endiannes.
         * 
         * @param value the ushort
         * @return PacketData& this instance
         */
        PacketData& operator<< (uint16_t value);

        /**
         * @brief Insert some raw bytes
         * 
         * @param rawBytes bytes to insert
         * @return PacketData& this instance
         */
        PacketData& operator<< (std::vector<uint8_t> const& rawBytes);

    private:
        std::vector<uint8_t> bytes;
};

}