#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"

namespace Minecraft{

struct ConfirmTeleportationPacket{
    VarInt teleportID;
    
    CODABLE_CONTENTS(
        teleportID
    )
};

}