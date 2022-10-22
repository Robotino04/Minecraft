#include "Minecraft/PacketCoder.hpp"

#include <vector>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    std::string value = ";:\\,.-+\"*%&/()=?'^";
    std::vector<uint8_t> correctResult = {
        0x12, ';', ':', '\\', ',', '.', '-', '+', '"', '*', '%', '&', '/', '(', ')', '=', '?', '\'', '^'
    };


    Minecraft::PacketCoder encoder;
    encoder << value;

    auto result = encoder.getBytes();

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