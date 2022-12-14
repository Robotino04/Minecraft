#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <functional>

#include <stdint.h>

#include "Minecraft/DataSink.hpp"

namespace Minecraft{

/**
 * @brief A write-only connection between a Minecraft server and client.
 * 
 */
class SendingConnectionEndpoint : public DataSink{
    public:
        /**
         * @brief Construct a new sending connection endpoint.
         */
        SendingConnectionEndpoint();
        /**
         * @brief Construct a new sending connection endpoint on the given file descripter.
         * 
         * This will issue a call to *start*.
         */
        SendingConnectionEndpoint(int fileDescriptor);
        
        /**
         * @brief Destroy the sending connection endpoint.
         * 
         * This will stop the thread responsible for sending the data.
         */
        ~SendingConnectionEndpoint();


        /**
         * @brief Start sending data.
         * 
         * @param fileDescriptor the socket connection to send to
         */
        void start(int fileDescriptor);

        using Callback = std::function<void(std::deque<uint8_t>::iterator begin, std::deque<uint8_t>::iterator end)>;

        /**
         * @brief Set the callback for sending bytes.
         * 
         * @param Callback the function to be called after bytes are sent
         */
        void setCallback(Callback callback);

        void pushByte(uint8_t byte) override;
        void pushBytes(std::deque<uint8_t> const& bytes) override;
    
    private:
        void spawnWorkerThread();
        void stopWorkerThread();


        int FD;

        std::deque<uint8_t> buffer;
        std::thread sendingThread;
        bool stopSending = false;

        Callback callback = [](auto, auto){};

        std::condition_variable conditionVar;
        std::mutex mutex;

};

}