#pragma once

#include <concepts>

namespace Minecraft::Utils::Concepts{

template <typename T>
concept integer = !std::is_same_v<T, bool> && std::is_integral_v<T>;

}