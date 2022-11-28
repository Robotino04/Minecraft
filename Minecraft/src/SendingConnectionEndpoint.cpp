#include "Minecraft/SendingConnectionEndpoint.hpp"

#include <sys/socket.h>

#include <array>

namespace Minecraft{

SendingConnectionEndpoint::SendingConnectionEndpoint(): FD(-1){
}
SendingConnectionEndpoint::SendingConnectionEndpoint(int fileDescriptor){
    start(fileDescriptor);
}

SendingConnectionEndpoint::~SendingConnectionEndpoint(){
    stopWorkerThread();
}


void SendingConnectionEndpoint::start(int fileDescriptor){
    FD = fileDescriptor;
    stopSending = false;
    spawnWorkerThread();
}

void SendingConnectionEndpoint::setCallback(Callback callback){
    this->callback = callback;
}

void SendingConnectionEndpoint::pushByte(uint8_t byte){
    std::unique_lock lock(mutex);

    buffer.push_back(byte);

    lock.unlock();
    conditionVar.notify_one();
}
void SendingConnectionEndpoint::pushBytes(std::deque<uint8_t> const& bytes){
    std::unique_lock lock(mutex);
    buffer.insert(buffer.end(), bytes.begin(), bytes.end());

    lock.unlock();
    conditionVar.notify_one();
}


void SendingConnectionEndpoint::spawnWorkerThread(){
    sendingThread = std::thread([&](){
        std::unique_lock lock(mutex);

        std::array<uint8_t, 1024> intermediateBuffer;

        while (!stopSending){
            conditionVar.wait(lock, [&](){return buffer.size() >= 1 || stopSending;});
            if (stopSending) break;

            size_t availableBytes = std::min(buffer.size(), intermediateBuffer.size());

            std::copy(buffer.begin(), std::next(buffer.begin(), availableBytes), intermediateBuffer.begin());

            int sentBytes = send(FD, intermediateBuffer.data(), availableBytes, 0);
            if (sentBytes == -1) continue;
            buffer.erase(buffer.begin(), std::next(buffer.begin(), sentBytes));
            
            callback(buffer.begin(), std::next(buffer.begin(), sentBytes));
        }
    });
}

void SendingConnectionEndpoint::stopWorkerThread(){
    stopSending = true;
    conditionVar.notify_one();
    if (sendingThread.joinable())
        sendingThread.join();
}

}