#include "Minecraft/PacketCoder.hpp"
#include "Minecraft/VarInt.hpp"
#include "Minecraft/Testing/TestSource.hpp"
#include "Minecraft/Utils/IO.hpp"

#include <vector>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    const std::deque<uint8_t> testedBytes = {
        0xff, 0xff, 0x7f
    };
    using TestedType = Minecraft::VarInt;
    const TestedType correctResult = 2097151;

    Minecraft::Testing::TestSource value(testedBytes);

    TestedType result;
    Minecraft::PacketCoder::decode(value, result);

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested bytes: {" << Minecraft::Utils::formatByteArray(testedBytes.begin(), testedBytes.end()) << "}\n";
        std::cout << "Correct result:   " << correctResult << "\n";
        std::cout << "Generated result: " << result << "\n";

        return 1;
    }
}