#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/ConnectionState.hpp"

#include <string>

namespace Minecraft{

struct HandshakePacket{
    VarInt protocolVersion;
    std::string host;
    uint16_t port;
    ConnectionState newConnectionState;

    CODABLE_CONTENTS(
        protocolVersion,
        host,
        port,
        newConnectionState
    )
};

}