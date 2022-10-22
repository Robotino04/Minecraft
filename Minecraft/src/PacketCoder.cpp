#include "Minecraft/PacketCoder.hpp"

#include <stdexcept>

namespace Minecraft{

PacketCoder::PacketCoder(){}

PacketCoder::PacketCoder(std::vector<uint8_t> const& data): bytes(data.begin(), data.end()){
}

std::vector<uint8_t> PacketCoder::getBytes() const{
    std::unique_lock lock(mutex);
    return {bytes.begin(), bytes.end()};
}

std::vector<uint8_t> PacketCoder::toPacket(PacketID id) const{
    std::unique_lock lock(mutex);
    PacketCoder encoder;

    encoder << static_cast<int32_t>(id);
    auto idSize = encoder.getBytes().size();

    PacketCoder packet;

    auto dataSize = bytes.size();
    auto length = idSize + dataSize;

    packet << static_cast<int32_t>(length) << static_cast<int32_t>(id) << bytes;

    return packet.getBytes();
}

size_t PacketCoder::getDecodedBytes(){
    std::unique_lock lock(mutex);
    auto x = decodedBytes;
    decodedBytes = 0;
    return x;
}
void PacketCoder::ignore(size_t numBytes){
    std::unique_lock lock(mutex);

    condition.wait(lock, [&](){return bytes.size() >= numBytes;});

    bytes.erase(bytes.begin(), std::next(bytes.begin(), numBytes));
}

size_t PacketCoder::size() const{
    std::unique_lock lock(mutex);
    return bytes.size();
}

PacketCoder& PacketCoder::operator<< (bool value){
    std::unique_lock lock(mutex);
    bytes.push_back(value? 0x01 : 0x00);

    lock.unlock();
    condition.notify_one();

    return *this;
}
PacketCoder& PacketCoder::operator<< (int32_t value){
    std::unique_lock lock(mutex);
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

    lock.unlock();
    condition.notify_one();

    return *this;
}
PacketCoder& PacketCoder::operator<< (std::string value){
    *this << static_cast<int32_t>(value.size());
    std::unique_lock lock(mutex);
    bytes.insert(bytes.end(), value.begin(), value.end());

    lock.unlock();
    condition.notify_one();

    return *this;
}
PacketCoder& PacketCoder::operator<< (uint16_t value){
    std::unique_lock lock(mutex);
    bytes.push_back(value >> 8);
    bytes.push_back(value & 0xFF);

    lock.unlock();
    condition.notify_one();
    
    return *this;
}
PacketCoder& PacketCoder::operator<< (std::vector<uint8_t> const& rawBytes){
    std::unique_lock lock(mutex);
    bytes.insert(bytes.end(), rawBytes.begin(), rawBytes.end());

    lock.unlock();
    condition.notify_one();
    

    return *this;
}
PacketCoder& PacketCoder::operator<< (std::deque<uint8_t> const& rawBytes){
    std::unique_lock lock(mutex);
    bytes.insert(bytes.end(), rawBytes.begin(), rawBytes.end());

    lock.unlock();
    condition.notify_one();

    return *this;
}


PacketCoder& PacketCoder::operator>> (bool& value){
    std::unique_lock lock(mutex);
    
    // wait for data
    condition.wait(lock, [&](){return !bytes.empty();});

    value = bytes.front();
    bytes.pop_front();

    decodedBytes++;

    return *this;
}
PacketCoder& PacketCoder::operator>> (int32_t& value){
    std::unique_lock lock(mutex);
    static constexpr uint8_t DATA_BITS = 0b01111111;
    static constexpr uint8_t CONTINUE_BIT = 0b10000000;

    uint32_t u_value = 0;
    int shiftAmount=7;
    uint8_t byte;
    do{
        // wait for data
        condition.wait(lock, [&](){return !bytes.empty();});
        
        byte
        =
        bytes.front();

        bytes.pop_front();
        u_value |= byte & DATA_BITS;
        u_value << 7;
        shiftAmount += 7;

        if (shiftAmount >= 32){
            throw std::runtime_error("VarInt to big!");
        }
        decodedBytes++;
    } while (byte & CONTINUE_BIT);
    value = static_cast<int32_t>(u_value);
    
    return *this;
}
PacketCoder& PacketCoder::operator>> (std::string& value){
    int32_t length;
    *this >> length;

    std::unique_lock lock(mutex);

    // wait for data
    condition.wait(lock, [&](){return bytes.size() >= length;});

    value.clear();
    value.reserve(length);

    value.insert(value.begin(), bytes.begin(), std::next(bytes.begin(), length));
    bytes.erase(bytes.begin(), std::next(bytes.begin(), length));

    decodedBytes += length;
    return *this;
}
PacketCoder& PacketCoder::operator>> (uint16_t& value){
    std::unique_lock lock(mutex);

    // wait for data
    condition.wait(lock, [&](){return bytes.size() >= 2;});

    value = bytes.front() << 8;
    bytes.pop_front();
    value |= bytes.front();
    bytes.pop_front();

    decodedBytes += 2;
    return *this;
}

}