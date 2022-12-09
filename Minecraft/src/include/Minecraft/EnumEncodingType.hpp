#pragma once

#include <type_traits>

namespace Minecraft{

/**
 * @brief The type to en-/decode an enum as.
 * 
 * @tparam T the enum
 */
template<typename T>
struct EnumCodingType{
    using type = std::void_t<>;
};

/**
 * @brief The type to en-/decode an enum as.
 * 
 * @tparam T the enum
 */
template<typename T>
using EnumCodingType_t = EnumCodingType<T>::type;

template<typename T>
concept CodableEnum = std::is_enum_v<T> && !std::is_void_v<EnumCodingType_t<T>>;
}