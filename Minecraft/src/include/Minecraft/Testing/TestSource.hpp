#pragma once

#include "Minecraft/DataSource.hpp"

#include <deque>
#include <stdint.h>

namespace Minecraft::Testing{

class TestSource : public DataSource{
    public:
        TestSource(std::deque<uint8_t> const& buffer): buffer(buffer){};
        
        uint8_t pullByte() override{
            if (buffer.empty())
                throw std::runtime_error("Test source was empty when pulling byte!");

            uint8_t byte = buffer.front();
            buffer.pop_front();
            return byte;
        }
        std::deque<uint8_t> pullBytes(size_t n) override{
            if (buffer.size() < n)
                throw std::runtime_error("Test source had " + std::to_string(buffer.size()) + " bytes when pulling " + std::to_string(n) + "!");
            std::deque<uint8_t> tmp;
            tmp.insert(tmp.begin(), buffer.begin(), std::next(buffer.begin(), n));
            buffer.erase(buffer.begin(), std::next(buffer.begin(), n));

            return tmp;
        }

        uint8_t readByte() const override{
            if (buffer.empty())
                throw std::runtime_error("Test source was empty when reading byte!");
            return buffer.front();
        }
        std::deque<uint8_t> readBytes(size_t n) const override{
            if (buffer.size() < n)
                throw std::runtime_error("Test source had " + std::to_string(buffer.size()) + " bytes when reading " + std::to_string(n) + "!");
            std::deque<uint8_t> tmp;
            tmp.insert(tmp.begin(), buffer.begin(), std::next(buffer.begin(), n));

            return tmp;
        }

        std::deque<uint8_t> buffer;
};

}