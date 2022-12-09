#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Angle.hpp"
#include "Minecraft/UUID.hpp"

namespace Minecraft{

struct SpawnPlayerPacket{
    VarInt entityID;
    UUID playerUUID;
    double x, y, z;
    Angle yaw, pitch;

    CODABLE_CONTENTS(
        entityID,
        playerUUID,
        x, y, z,
        yaw, pitch
    )
};

}