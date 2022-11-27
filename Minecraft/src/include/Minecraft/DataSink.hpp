#pragma once

#include <stdint.h>
#include <deque>

namespace Minecraft{

class DataSink{
    public:
        /**
         * @brief Push a new byte
         * 
         * @param byte the byte to be pushed
         */
        virtual void pushByte(uint8_t byte) = 0;

        /**
         * @brief Push multiple bytes at once.
         * 
         * Provides a simple default implementation but should
         * be overrridden if better implementations are available.
         * 
         * @param bytes the bytes to be pushed
         */
        virtual void pushBytes(std::deque<uint8_t> const& bytes){
            for (auto byte : bytes)
                pushByte(byte);
        }
};

}