#include "Minecraft/PacketCoder.hpp"
#include "Minecraft/Testing/TestSource.hpp"
#include "Minecraft/Utils/IO.hpp"
#include "Minecraft/Position.hpp"

#include <vector>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    const std::deque<uint8_t> testedBytes = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    using TestedType = Minecraft::Position;
    const TestedType correctResult{
        .x = 0,
        .y = 0,
        .z = 0
    };

    Minecraft::Testing::TestSource value(testedBytes);

    TestedType result;
    Minecraft::PacketCoder::decode(value, result);

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested bytes: {" << Minecraft::Utils::formatByteArray(testedBytes.begin(), testedBytes.end()) << "}\n";
        std::cout << "Correct result:   " << Minecraft::Utils::formatPositioned(correctResult) << "\n";
        std::cout << "Generated result: " << Minecraft::Utils::formatPositioned(result) << "\n";

        return 1;
    }
}