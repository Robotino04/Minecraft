#include "Minecraft/PacketData.hpp"

namespace Minecraft{

PacketData::PacketData(){}

PacketData::PacketData(size_t size){
    bytes.reserve(size);
}

PacketData::PacketData(std::vector<uint8_t> const& data): bytes(data){
}

std::vector<uint8_t> PacketData::getBytes() const{
    return bytes;
}

PacketData PacketData::toPacket(PacketID id) const{
    PacketData packet;

    packet << static_cast<int32_t>(id);
    auto idSize = packet.getBytes().size();
    packet = PacketData();

    auto dataSize = bytes.size();
    auto length = idSize + dataSize;

    packet << static_cast<int32_t>(length) << static_cast<int32_t>(id) << bytes;

    return packet;
}


PacketData& PacketData::operator<< (bool value){
    bytes.push_back(value? 0x01 : 0x00);

    return *this;
}
PacketData& PacketData::operator<< (int32_t value){
    static constexpr uint8_t DATA_BITS = 0b01111111;
    static constexpr uint8_t CONTINUE_BIT = 0b10000000;

    uint32_t u_value = static_cast<uint32_t>(value);

    do {
        auto& byte = bytes.emplace_back();
        byte = u_value & DATA_BITS;
        u_value &= (uint32_t)~DATA_BITS;

        byte |= (u_value? 1 : 0) << 7;
        u_value >>= 7;
    } while (u_value);

    return *this;
}
PacketData& PacketData::operator<< (std::string value){
    *this << static_cast<int32_t>(value.size());
    bytes.insert(bytes.end(), value.begin(), value.end());

    return *this;
}
PacketData& PacketData::operator<< (uint16_t value){
    bytes.push_back(value >> 8);
    bytes.push_back(value & 0xFF);

    return *this;
}
PacketData& PacketData::operator<< (std::vector<uint8_t> const& rawBytes){
    bytes.insert(bytes.end(), rawBytes.begin(), rawBytes.end());

    return *this;
}

}