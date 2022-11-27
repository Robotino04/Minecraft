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
    using TestedType = std::vector<uint8_t>;
    const TestedType correctResult = {};
    Minecraft::Testing::TestSource value(testedBytes);

    TestedType result;
    result.resize(correctResult.size()); // ensure size 0
    

    Minecraft::PacketCoder::decode(value, result);

    if (result == correctResult)
        return 0;
    else{
        std::cout << "Tested bytes: {" << Minecraft::Utils::formatByteArray(testedBytes.begin(), testedBytes.end()) << "}\n";
        
        std::cout << "Correct result: Size: " << correctResult.size() << ", Contents: {" << Minecraft::Utils::formatVectorContents(correctResult) << "}\n";
        std::cout << "Generated result: Size: " << result.size() << ", Contents: {" << Minecraft::Utils::formatVectorContents(result) << "}\n";

        return 1;
    }
}