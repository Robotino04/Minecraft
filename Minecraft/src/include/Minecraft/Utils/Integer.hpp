#pragma once

#include <stdint.h>
#include <type_traits>

namespace Minecraft::Utils{

namespace Detail{
    template<unsigned int SIZE>
    struct __Integer;

    template<>
    struct __Integer<8>{using type = int8_t;};
    template<>
    struct __Integer<16>{using type = int16_t;};
    template<>
    struct __Integer<32>{using type = int32_t;};
    template<>
    struct __Integer<64>{using type = int64_t;};
}

/**
 * @brief An integer able to hold at least SIZE bits
 * 
 * @tparam SIZE minimal number of bits 
 */
template<int SIZE>
using Integer = typename Detail::__Integer<SIZE>::type;


/**
 * @brief An unsigned integer able to hold at least SIZE bits
 * 
 * @tparam SIZE minimal number of bits 
 */
template<int SIZE>
using UnsignedInteger = std::make_unsigned_t<Integer<SIZE>>;

}