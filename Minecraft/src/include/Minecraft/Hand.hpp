#pragma once

#include "Minecraft/EnumEncodingType.hpp"

namespace Minecraft{

enum Hand{
    MainHand = 0,
    OffHand = 1
};

template<>
struct EnumCodingType<Hand>{
    using type = VarInt;
};

}