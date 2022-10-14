#include "Minecraft/VarInt.hpp"

#include <vector>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    int value = 25565;

    std::vector<uint8_t> correctResult = {
        0xdd, 0xc7, 0x01
    };


    Minecraft::VarInt testInt(value);

    auto result = testInt.toBytes();

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested value: " << value << "\n";

        std::cout << "Correct bytes:" << std::hex;
        for (auto byte : correctResult){
            std::cout << " 0x" << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        std::cout << std::dec << "\n";

        std::cout << "Generated bytes:" << std::hex;
        for (auto byte : result){
            std::cout << " 0x" << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        std::cout << std::dec << "\n";

        return 1;
    }
}