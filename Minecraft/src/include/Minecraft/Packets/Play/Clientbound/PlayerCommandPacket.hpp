#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/EnumEncodingType.hpp"
#include "Minecraft/PlayerCommandAction.hpp"


namespace Minecraft{

struct PlayerCommandPacket{
    VarInt entityID;
    PlayerCommandAction actionID;
    VarInt jumpBoost;

    CODABLE_CONTENTS(
        entityID,
        actionID,
        jumpBoost
    )
};

}