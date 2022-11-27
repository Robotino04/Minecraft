#include "Minecraft/PacketCoder.hpp"
#include "Minecraft/Testing/TestSource.hpp"
#include "Minecraft/Conditional.hpp"
#include "Minecraft/Utils/IO.hpp"

#include <vector>
#include <stdint.h>
#include <iostream>
#include <iomanip>

int main(){
    const std::deque<uint8_t> testedBytes = {
        
    };
    bool correctIsPresent = false;
    using TestedType = Minecraft::Conditional<uint8_t>;
    const TestedType correctResult(correctIsPresent, 0xA5);
    Minecraft::Testing::TestSource value(testedBytes);

    bool isPresent = false;
    TestedType result(isPresent);
    Minecraft::PacketCoder::decode(value, result);

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested bytes: {" << Minecraft::Utils::formatByteArray(testedBytes.begin(), testedBytes.end()) << "}\n";
        
        std::cout << "Correct result: Is Present: " << std::boolalpha << correctResult.isPresent() << ", Value: " << static_cast<int>(correctResult.value) << "\n";
        std::cout << "Generated result: Is Present: " << std::boolalpha << result.isPresent() << ", Value: " << static_cast<int>(result.value) << "\n";

        return 1;
    }
}