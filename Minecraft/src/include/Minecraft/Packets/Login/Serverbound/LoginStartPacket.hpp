#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/Conditional.hpp"
#include "Minecraft/VarInt.hpp"

#include <string>
#include <vector>

namespace Minecraft{

struct LoginStartPacket{
    std::string name;
    bool hasSigData;
    Minecraft::Conditional<int64_t> timestamp = Minecraft::Conditional<int64_t>(hasSigData);
    Minecraft::Conditional<Minecraft::VarInt> publicKeyLength = Minecraft::Conditional<Minecraft::VarInt>(hasSigData);
    Minecraft::Conditional<std::vector<uint8_t>> publicKey = Minecraft::Conditional<std::vector<uint8_t>>(hasSigData);
    Minecraft::Conditional<Minecraft::VarInt> signatureLength = Minecraft::Conditional<Minecraft::VarInt>(hasSigData);
    Minecraft::Conditional<std::vector<uint8_t>> signature = Minecraft::Conditional<std::vector<uint8_t>>(hasSigData);
    bool hasPlayerUUID;
    // Minecraft::Conditional<Minecraft::UUID> playerUUID = Minecraft::Conditional<Minecraft::UUID>(hasPlayerUUID);


    CODABLE_CONTENTS(
        name,
        hasSigData,
        timestamp,
        publicKeyLength,
        publicKey,
        signatureLength,
        signature,
        hasPlayerUUID
    )
};

}