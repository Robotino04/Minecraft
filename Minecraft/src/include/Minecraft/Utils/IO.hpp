#pragma once

#include <sstream>
#include <string>
#include <stdint.h>
#include <vector>
#include <iomanip>
#include <ios>
#include <utility>

namespace Minecraft::Utils{

inline std::string formatByteArray(auto begin, auto end){
    if (begin == end) return "";
    std::stringstream ss;
    ss << std::hex;
    for (auto byteIt = begin; byteIt != end; byteIt++){
        ss << "0x" << std::setw(2) << std::uppercase << std::setfill('0') << static_cast<int>(*byteIt) << " ";
    }
    std::string s = ss.str();
    s.erase(std::prev(s.end()));
    return s;
}

template<typename T>
inline std::string formatVectorContents(std::vector<T> const& vec){
    if (vec.empty()) return "";
    std::stringstream ss;
    for (auto const& element : vec){
        if constexpr(std::is_same_v<T, uint8_t>){
            ss << static_cast<int>(element) << ", ";
        }
        else {
            ss << element << ", ";
        }
    }

    std::string s = ss.str();
    s.erase(std::prev(s.end()));
    s.erase(std::prev(s.end()));
    return s;
}

template<typename T>
concept Positioned = requires{
    std::declval<T>().x;
    std::declval<T>().y;
    std::declval<T>().z;
};

template<Positioned T>
inline std::string formatPositioned(T const& thing){
    return "X: " + std::to_string(thing.x) + " Y: " + std::to_string(thing.y) + " Z: " + std::to_string(thing.z);
}

}