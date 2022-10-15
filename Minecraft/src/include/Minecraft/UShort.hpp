#pragma once

#include "Minecraft/PacketData.hpp"

namespace Minecraft{

/**
 * @brief Unsigned short 
 * 
 * An unsigned short convertable to bytes.
 */
class UShort : public PacketData{
    public:
        /**
         * @brief Construct a new UShort from a normal unsigned short
         * 
         * @param value the number represented by the UShort
         */
        UShort(uint16_t value);

        /**
         * @brief Convert to byte array.
         * 
         * Converts the internal uint16_t to the
         * correct endianness.
         * 
         * @see https://wiki.vg/Protocol#Data_types
         * 
         * @return std::vector<uint8_t> the byte array
         * 
         */
        std::vector<uint8_t> toBytes() const override;
    private:
        uint16_t value;
};

}