#include "Minecraft/VarInt.hpp"

namespace Minecraft{

VarInt::VarInt(int value){
    this->value = value;
}

std::vector<uint8_t> VarInt::toBytes() const{
    std::vector<uint8_t> bytes;
    bytes.reserve(5);
    uint32_t u_value = static_cast<uint32_t>(value);

    do {
        auto& byte = bytes.emplace_back();
        byte = u_value & DATA_BITS;
        u_value &= (uint32_t)~DATA_BITS;

        byte |= (u_value?1:0) << 7;
        u_value >>= 7;
    } while (u_value);
    return bytes;
}

}