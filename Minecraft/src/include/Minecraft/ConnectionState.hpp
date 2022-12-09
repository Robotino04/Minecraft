#pragma once

#include "Minecraft/EnumEncodingType.hpp"
#include "Minecraft/VarInt.hpp"

namespace Minecraft{

enum class ConnectionState{
    Handshaking = 0,
    Status = 1,
    Login = 2,
    Play = 3,
};

template<>
struct EnumCodingType<ConnectionState>{
    using type = VarInt;
};

}