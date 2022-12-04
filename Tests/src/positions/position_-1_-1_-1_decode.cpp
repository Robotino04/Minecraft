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
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    using TestedType = Minecraft::Position;
    const TestedType correctResult{
        .x = -1,
        .y = -1,
        .z = -1
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