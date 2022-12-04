#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"

namespace Minecraft{

struct SynchronizePlayerPositionPacket{
    double x, y, z;
    float yaw, pitch;
    uint8_t flags;
    Minecraft::VarInt teleportID;
    bool dismountVehicle;

    CODABLE_CONTENTS(
        x, y, z,
        yaw, pitch,
        flags,
        teleportID,
        dismountVehicle
    )
};

}