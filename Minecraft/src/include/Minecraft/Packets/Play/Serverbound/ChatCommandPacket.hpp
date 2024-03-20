#pragma once

#include "Minecraft/Codable.hpp"
#include "Minecraft/VarInt.hpp"

#include <string>
#include <vector>

namespace Minecraft{

struct SignedChatCommandArguments{
    std::string name;
    VarInt signatureLength;
    std::vector<uint8_t> signature;

    CODABLE_CONTENTS(
        name,
        signatureLength,
        signature
    )
};

struct ChatCommandPacket{
    std::string command;
    int64_t timestamp;
    int64_t salt;
    VarInt numSignedArguments;
    std::vector<SignedChatCommandArguments> signedArguments;
    bool signedPreview;

    CODABLE_CONTENTS(
        command,
        timestamp,
        salt,
        numSignedArguments,
        signedArguments,
        signedPreview
    )
};

}