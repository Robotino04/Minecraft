#pragma once

#include <stdint.h>

namespace Minecraft{

struct Position{
    int64_t x, y, z;

    bool operator == (Position const& other) const{
        return this->x == other.x && this->y == other.y && this->z == other.z;
    }
};

}