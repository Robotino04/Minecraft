#pragma once

#include "Minecraft/PacketData.hpp"

namespace Minecraft{

/**
 * @brief Variable sized integer.
 * 
 * Integer storing an arbitrarily big
 * number. Minecraft specifies a Var Int
 * may only be up to 5 bytes big.
 */
class VarInt : public PacketData{
    public:
        /**
         * @brief Construct a new Var Int from a normal int
         * 
         * @param value the number represented by the Var Int
         */
        VarInt(int value);

        /**
         * @brief Convert to byte array.
         * 
         * Converts the internal int32_t to the
         * Var Int encoding used by Minecraft.
         * 
         * @see https://wiki.vg/Protocol#VarInt_and_VarLong
         * 
         * @return std::vector<uint8_t> the byte array
         * 
         */
        std::vector<uint8_t> toBytes() const override;
    private:
        int32_t value;

        static constexpr uint8_t DATA_BITS = 0b01111111;
        static constexpr uint8_t CONTINUE_BIT = 0b10000000;
};

}