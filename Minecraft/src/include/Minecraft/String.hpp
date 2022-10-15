#pragma once

#include "Minecraft/PacketData.hpp"
#include "Minecraft/VarInt.hpp"

#include <string>

namespace Minecraft{

class String : public PacketData{
    public:
        /**
         * @brief Construct a new Minecraft String from an std::string
         * 
         * @param str the string to be represented
         */
        String(std::string const& str);

        /**
         * @brief Convert to byte array.
         * 
         * Converts the internal std::string to
         * the Minecraft format:
         *  - A Var Int containing the string size in bytes
         *  - The raw bytes of the string
         * 
         * @see https://wiki.vg/Protocol#Data_types
         * 
         * @return std::vector<uint8_t> the byte array
         * 
         */
        std::vector<uint8_t> toBytes() const override;
    
    private:
        VarInt length;
        std::string str;
};

}