#pragma once

#include "Minecraft/Codable.hpp"

namespace Minecraft{

struct SetPlayerPositionPacket{
    double x, y, z;
    bool onGround = true;

    CODABLE_CONTENTS(
        x, y, z,
        onGround
    )
};

}