#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Angle.hpp"

namespace Minecraft{

struct TeleportEntityPacket{
    VarInt entityID;
    double x, y, z;
    Angle yaw, pitch;
    bool onGround;

    CODABLE_CONTENTS(
        entityID,
        x, y, z,
        yaw, pitch,
        onGround
    )
};

}