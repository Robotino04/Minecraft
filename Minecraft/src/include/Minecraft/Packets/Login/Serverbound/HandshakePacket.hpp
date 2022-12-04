#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"

#include <string>

namespace Minecraft{

struct HandshakePacket{
    Minecraft::VarInt protocolVersion;
    std::string host;
    uint16_t port;
    Minecraft::VarInt newConnectionState;

    CODABLE_CONTENTS(
        protocolVersion,
        host,
        port,
        newConnectionState
    )
};

}