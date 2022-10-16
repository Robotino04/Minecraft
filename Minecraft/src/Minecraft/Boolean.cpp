#include "Minecraft/Boolean.hpp"

namespace Minecraft{

Boolean::Boolean(bool value){
    this->value = value;
}

std::vector<uint8_t> Boolean::toBytes() const{
    return {(uint8_t)(value ? 0x01 : 0x00)};
}

}