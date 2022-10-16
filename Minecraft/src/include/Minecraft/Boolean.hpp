#pragma once

#include "Minecraft/PacketData.hpp"

namespace Minecraft{

/**
 * @brief Unsigned short 
 * 
 * An unsigned short convertable to bytes.
 */
class Boolean : public PacketData{
    public:
        /**
         * @brief Construct a new Boolean from a normal bool
         * 
         * @param value the value represented by the Boolean
         */
        Boolean(bool value);

        /**
         * @brief Convert to byte array.
         * 
         * Converts the internal bool to a Minecraft Boolean
         * 
         * @see https://wiki.vg/Protocol#Data_types
         * 
         * @return std::vector<uint8_t> the byte array
         * 
         */
        std::vector<uint8_t> toBytes() const override;
    private:
        bool value;
};

}