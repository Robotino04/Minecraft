#pragma once

#include <stdint.h>

#include "Minecraft/Utils/Integer.hpp"

namespace Minecraft {

template<uint SIZE>
struct VarInteger{
    public:
        using type = Utils::Integer<SIZE>;

        VarInteger(): value(0)              {}
        VarInteger(type value): value(value){}
        type operator = (type newValue)     {return value = newValue;}
        
        operator type()                     {return value;}
        operator type() const               {return value;}

        type value;
};

typedef VarInteger<64> VarLong;
typedef VarInteger<32> VarInt;

}