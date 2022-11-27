#include "Minecraft/PacketCoder.hpp"
#include "Minecraft/Testing/TestSink.hpp"
#include "Minecraft/Utils/IO.hpp"

#include <deque>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    const int16_t value = 1;
    const std::deque<uint8_t> correctResult = {
        0x00, 0x01
    };


    std::deque<uint8_t> result;

    Minecraft::PacketCoder::encode(result, value);

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested value: " << value << "\n";

        std::cout << "Correct bytes:   {" << Minecraft::Utils::formatByteArray(correctResult.begin(), correctResult.end()) << "}\n";

        std::cout << "Generated bytes: {" << Minecraft::Utils::formatByteArray(result.begin(), result.end()) << "}\n";

        return 1;
    }
}