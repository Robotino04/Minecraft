#include "Minecraft/String.hpp"

namespace Minecraft{

String::String(std::string const& str): length(str.size()){
    this->str = str;
}


std::vector<uint8_t> String::toBytes() const{
    auto bytes = length.toBytes();
    bytes.insert(bytes.end(), str.begin(), str.end());
    return bytes;
}

}