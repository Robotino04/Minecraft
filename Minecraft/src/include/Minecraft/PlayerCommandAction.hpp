#pragma once
#include "Minecraft/EnumEncodingType.hpp"
#include "Minecraft/VarInt.hpp"

namespace Minecraft{

enum class PlayerCommandAction{
    startSneaking = 0,
    stopSneaking = 1,
    leaveBed = 2,
    startSprinting = 3,
    stopSprinting = 4,
    startJumpWithHorse = 5,
    stopJumpWithHorse = 6,
    openHorseInventory = 7,
    startFlyingWithElytra = 8,
};

template<>
struct EnumCodingType<PlayerCommandAction>{
    using type = VarInt;
};

}