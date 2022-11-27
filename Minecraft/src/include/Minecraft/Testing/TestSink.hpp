#pragma once

#include "Minecraft/DataSink.hpp"

#include <deque>
#include <stdint.h>

namespace Minecraft::Testing{

class TestSink : public DataSink{
    public:
        void pushByte(uint8_t byte) override{
            buffer.push_back(byte);
        }
        void pushBytes(std::deque<uint8_t> const& bytes) override{
            buffer.insert(buffer.end(), bytes.begin(), bytes.end());
        }

        std::deque<uint8_t> buffer;
};

}