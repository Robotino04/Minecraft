#include "Minecraft/PacketCoder.hpp"
#include "Minecraft/Testing/TestSink.hpp"
#include "Minecraft/Utils/IO.hpp"

#include <deque>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    const Minecraft::Position value{
        .x = -1239,
        .y = -817,
        .z = -9917
    };
    const std::deque<uint8_t> correctResult = {
        0xFF, 0xFE, 0xCA, 0x7F, 0xFD, 0x94, 0x3C, 0xCF
    };


    std::deque<uint8_t> result;

    Minecraft::PacketCoder::encode(result, value);

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested value: " << Minecraft::Utils::formatPositioned(value) << "\n";

        std::cout << "Correct bytes:   {" << Minecraft::Utils::formatByteArray(correctResult.begin(), correctResult.end()) << "}\n";
        std::cout << "Generated bytes: {" << Minecraft::Utils::formatByteArray(result.begin(), result.end()) << "}\n";

        return 1;
    }
}