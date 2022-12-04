#pragma once

#include "Minecraft/Codable.hpp"

#include <array>
#include <stdint.h>

namespace Minecraft{

struct UUID{
    std::array<uint64_t, 2> value;

    bool operator == (UUID const& other) const{
        return this->value == other.value;
    }

    CODABLE_CONTENTS(
        value.at(0),
        value.at(1)
    )
};

}