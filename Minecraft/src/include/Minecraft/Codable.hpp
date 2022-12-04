#pragma once

#include <utility>
#include <tuple>

namespace Minecraft{

/**
 * @brief An en- and decodable thing
 * 
 * Must have a ```contents()``` function returning a
 * tuple of the data to be processed.
 */
template<typename T>
concept Codable = requires{
    std::declval<T>().contents();
    std::declval<const T>().contents();
};

/**
 * @brief Declare all functions to make a struct a *Codable*.
 * 
 */
#define CODABLE_CONTENTS(...) \
    auto contents() { \
        return std::forward_as_tuple( \
            __VA_ARGS__ \
        ); \
    } \
    auto contents() const{ \
        return std::forward_as_tuple( \
            __VA_ARGS__ \
        ); \
    }
}