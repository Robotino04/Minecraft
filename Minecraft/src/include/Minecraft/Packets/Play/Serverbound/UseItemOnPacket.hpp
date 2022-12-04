#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Position.hpp"

namespace Minecraft{

struct UseItemOnPacket{
    Minecraft::VarInt hand;
    Minecraft::Position location;
    Minecraft::VarInt face;
    float cursorPositionX, cursorPositionY, cursorPositionZ;
    bool insideBlock;
    Minecraft::VarInt sequence;

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