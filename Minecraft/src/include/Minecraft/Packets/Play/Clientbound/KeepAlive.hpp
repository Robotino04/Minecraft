#pragma once

#include "Minecraft/Codable.hpp"

#include <stdint.h>

namespace Minecraft{

struct KeepAlivePacket{
    int64_t keepAliveID;

    CODABLE_CONTENTS(
        keepAliveID
    )
};

}