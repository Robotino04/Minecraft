#pragma once

#include <concepts>

namespace Minecraft::Utils::Concepts{

template <typename T>
concept Integer = !std::is_same_v<T, bool> && std::is_integral_v<T>;
template <typename T>
concept Enum = std::is_enum_v<T>;

}