#pragma once

#include <stdint.h>
#include <deque>

namespace Minecraft{

class DataSource{
    public:
        /**
         * @brief Return the next byte to be processed and remove it.
         * 
         * @return uint8_t the pulled byte
         */
        virtual uint8_t pullByte() = 0;

        /**
         * @brief Return the next bytes to be processed and remove them.
         * 
         * Provides a simple default implementation but should
         * be overrridden if better implementations are available.
         * 
         * @param n the number of bytes
         * @return std::deque<uint8_t>&& the pulled bytes 
         */
        virtual std::deque<uint8_t> pullBytes(size_t n){
            std::deque<uint8_t> bytes;
            for(size_t i=0; i<n; i++){
                bytes.push_back(pullByte());
            }
            return bytes;
        }

        /**
         * @brief Return the next byte to be processed.
         * 
         * @return uint8_t the read byte
         */
        virtual uint8_t readByte() const = 0;

        /**
         * 
         * @brief Return the next bytes to be processed.
         * 
         * Provides a simple default implementation but should
         * be overrridden if better implementations are available.
         * 
         * @param n the number of bytes
         * @return std::deque<uint8_t>&& the read bytes 
         */
        virtual std::deque<uint8_t> readBytes(size_t n) const{
            std::deque<uint8_t> bytes;
            for(size_t i=0; i<n; i++){
                bytes.push_back(readByte());
            }
            return bytes;
        }
};

}