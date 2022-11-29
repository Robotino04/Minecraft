#pragma once


#include <stdint.h>
#include <algorithm>
#include <bit>
#include <deque>
#include <memory.h>
#include <array>
#include <assert.h>

namespace Minecraft::Utils{

template <typename Iterator>
void correctEndianness(Iterator begin, Iterator end){
    if constexpr(std::endian::native != std::endian::big){
        std::reverse(begin, end);
    }
}

template <typename T>
std::deque<uint8_t> getRawBytes(T const& t){
    std::array<uint8_t, sizeof(T)> result;
    memcpy(result.data(), &t, sizeof(T));

    return result;
}

template <typename T>
void setRawBytes(T& t, std::deque<uint8_t> const& bytes){
    assert(sizeof(T) == bytes.size());

    std::array<uint8_t, sizeof(T)> result = {bytes.begin(), bytes.end()};
    memcpy(&t, result.data(), sizeof(T));
}



}