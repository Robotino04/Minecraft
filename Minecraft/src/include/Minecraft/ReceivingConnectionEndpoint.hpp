#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>

#include <stdint.h>

#include "Minecraft/DataSource.hpp"

namespace Minecraft{

/**
 * @brief A write-only connection between a Minecraft server and client.
 * 
 */
class ReceivingConnectionEndpoint : public DataSource{
    public:
        /**
         * @brief Construct a new Receiving Connection Endpoint object.
         */
        ReceivingConnectionEndpoint();

        /**
         * @brief Construct a new Receiving Connection Endpoint object on the given file descriptor.
         * 
         * This issue a call to *start*.
         */
        ReceivingConnectionEndpoint(int fileDescriptor);


        /**
         * @brief Destroy the receiving connection endpoint.
         * 
         * This will stop the thread responsible for reading the data.
         */
        ~ReceivingConnectionEndpoint();
        

        /**
         * @brief Start reading incomming data into the buffer.
         * 
         * @param fileDescriptor the socket connection to receive data from
         */
        void start(int fileDescriptor);

        using Callback = std::function<void(std::deque<uint8_t>::iterator begin, std::deque<uint8_t>::iterator end)>;

        /**
         * @brief Set the callback for receiving bytes.
         * 
         * @param Callback the function to be called when new bytes are received
         */
        void setCallback(Callback callback);
        
        uint8_t pullByte() override;
        std::deque<uint8_t> pullBytes(size_t n) override;
        uint8_t readByte() const override;
        std::deque<uint8_t> readBytes(size_t n) const override;
    private:
        void spawnWorkerThread();
        void stopWorkerThread();
        

        int FD;

        Callback callback = [](auto, auto){};;

        std::deque<uint8_t> buffer;
        std::thread sendingThread;
        bool stopSending = false;

        mutable std::condition_variable conditionVar;
        mutable std::mutex mutex;

};

}