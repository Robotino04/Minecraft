#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/Conditional.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Conditional.hpp"

#include <string>
#include <vector>

namespace Minecraft{

struct LoginStartPacket{
    std::string name;
    bool hasSigData;
    Conditional<int64_t> timestamp = Conditional<int64_t>(hasSigData);
    Conditional<VarInt> publicKeyLength = Conditional<VarInt>(hasSigData);
    Conditional<std::vector<uint8_t>> publicKey = Conditional<std::vector<uint8_t>>(hasSigData);
    Conditional<VarInt> signatureLength = Conditional<VarInt>(hasSigData);
    Conditional<std::vector<uint8_t>> signature = Conditional<std::vector<uint8_t>>(hasSigData);
    bool hasPlayerUUID;
    Conditional<UUID> playerUUID = Conditional<UUID>(hasPlayerUUID);


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