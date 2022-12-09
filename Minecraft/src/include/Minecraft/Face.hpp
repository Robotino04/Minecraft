#pragma  once

#include "Minecraft/EnumEncodingType.hpp"

namespace Minecraft{

enum Face{
    Bottom = 0,
    Top = 1,
    North = 2,
    South = 3,
    West = 4,
    East = 5,
};

template<>
struct EnumCodingType<Face>{
    using type = VarInt;
};
}