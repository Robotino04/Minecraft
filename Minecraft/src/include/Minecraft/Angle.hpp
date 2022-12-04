#pragma once

#include "Minecraft/Codable.hpp"

#include <stdint.h>

namespace Minecraft{

struct Angle{
    uint8_t value;

    bool operator == (Angle const& other) const{
        return this->value == other.value;
    }

    CODABLE_CONTENTS(
        value
    )
};

}