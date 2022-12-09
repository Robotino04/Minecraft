#pragma once

#include <stdint.h>
#include <type_traits>

#include "Minecraft/Utils/Integer.hpp"
#include "Minecraft/Utils/Concepts.hpp"

namespace Minecraft {

template<unsigned int SIZE>
struct VarInteger{
    public:
        using type = Utils::Integer<SIZE>;

        VarInteger(): value(0)              {}
        VarInteger(type value): value(value){}
        type operator = (type newValue)     {return value = newValue;}

        template<Utils::Concepts::Enum E>
        VarInteger(E e): value(static_cast<std::underlying_type_t<E>>(e)){}
        
        operator type()                     {return value;}
        operator type() const               {return value;}

        type value;
};

typedef VarInteger<64> VarLong;
typedef VarInteger<32> VarInt;

}