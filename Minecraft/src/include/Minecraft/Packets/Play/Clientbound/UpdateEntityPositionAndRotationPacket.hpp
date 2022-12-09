#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Angle.hpp"

namespace Minecraft{

struct UpdateEntityPositionAndRotationPacket{
    VarInt entityID;
    int16_t dX, dY, dZ;
    Angle yaw, pitch;
    bool onGround;

    CODABLE_CONTENTS(
        entityID,
        dX, dY, dZ,
        yaw, pitch,
        onGround
    )
};

}