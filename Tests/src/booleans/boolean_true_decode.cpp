#include "Minecraft/PacketCoder.hpp"

#include <vector>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    const std::vector<uint8_t> value = {
        0x01
    };
    const bool correctResult = true;


    Minecraft::PacketCoder encoder;
    encoder << value;

    bool result;
    encoder >> result;

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested bytes:" << std::hex;
        for (auto byte : value){
            std::cout << " 0x" << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        std::cout << std::dec << "\n";
        std::cout << "Correct result: " << std::boolalpha << correctResult << "\n";
        std::cout << "Generated result: " << std::boolalpha << result << "\n";

        return 1;
    }
}