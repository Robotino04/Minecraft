#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"

namespace Minecraft{

struct SetHealthPacket{
    float health;
    Minecraft::VarInt food;
    float foodSaturation;

    CODABLE_CONTENTS(
        health,
        food,
        foodSaturation
    )
};

}