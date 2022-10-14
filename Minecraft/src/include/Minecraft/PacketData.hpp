#pragma once

#include <vector>
#include <stdint.h>

namespace Minecraft{

/**
 * @brief Data sendable using a packet.
 * 
 * A base class for any kind of data that
 * may be sent between server and client.
 */
class PacketData{
    public:
        /**
         * @brief Convert to byte array
         * 
         * Converts the data from an internal
         * representation to an array of bytes which
         * can be sent using a packet.
         * 
         * @return std::vector<uint8_t> the byte array
         */
        virtual std::vector<uint8_t> toBytes() const = 0;
};

}