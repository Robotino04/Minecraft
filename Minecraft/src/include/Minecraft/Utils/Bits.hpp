#pragma once

#include <stdint.h>

namespace Minecraft::Utils{

template<int ONES>
constexpr auto binaryOnes = binaryOnes<ONES-1> << 1 | 1;

template<>
constexpr auto binaryOnes<0> = 0;


}