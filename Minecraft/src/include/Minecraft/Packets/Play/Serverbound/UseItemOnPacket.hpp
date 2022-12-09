#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Position.hpp"
#include "Minecraft/Face.hpp"
#include "Minecraft/Hand.hpp"

namespace Minecraft{

struct UseItemOnPacket{
    Hand hand;
    Position location;
    Face face;
    float cursorPositionX, cursorPositionY, cursorPositionZ;
    bool insideBlock;
    VarInt sequence;

    CODABLE_CONTENTS(
        hand,
        location,
        face,
        cursorPositionX, cursorPositionY, cursorPositionZ,
        insideBlock,
        sequence
    )
};

}