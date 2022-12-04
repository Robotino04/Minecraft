#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"

namespace Minecraft{

struct UpdateEntityPositionPacket{
    Minecraft::VarInt entityID;
    int16_t dX, dY, dZ;
    bool onGround;

    CODABLE_CONTENTS(
        entityID,
        dX, dY, dZ,
        onGround
    )
};

}