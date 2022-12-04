#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Angle.hpp"

namespace Minecraft{

struct UpdateEntityRotationPacket{
    Minecraft::VarInt entityID;
    Minecraft::Angle yaw, pitch;
    bool onGround;

    CODABLE_CONTENTS(
        entityID,
        yaw, pitch,
        onGround
    )
};

}