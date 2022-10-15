#include "Minecraft/UShort.hpp"

namespace Minecraft{

UShort::UShort(uint16_t value){
    this->value = value;
}

std::vector<uint8_t> UShort::toBytes() const{
    return {(uint8_t)(value >> 8), (uint8_t)(value & 0xFF)};
}

}