#include "Minecraft/PacketCoder.hpp"

#include <vector>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    const std::vector<uint8_t> value = {
        0xff, 0xff, 0xff, 0xff, 0x07
    };
    const int32_t correctResult = 2147483647;


    Minecraft::PacketCoder encoder;
    encoder << value;

    int32_t result;
    encoder >> result;

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested bytes:" << std::hex;
        for (auto byte : value){
            std::cout << " 0x" << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        std::cout << std::dec << "\n";
        std::cout << "Correct result: " << correctResult << "\n";
        std::cout << "Generated result: " << result << "\n";

        return 1;
    }
}