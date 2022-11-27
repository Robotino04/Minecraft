#include "Minecraft/ReceivingConnectionEndpoint.hpp"

#include <sys/socket.h>

#include <array>

namespace Minecraft{

ReceivingConnectionEndpoint::ReceivingConnectionEndpoint(): FD(-1){
}
ReceivingConnectionEndpoint::ReceivingConnectionEndpoint(int fileDescriptor){
    start(fileDescriptor);
}

ReceivingConnectionEndpoint::~ReceivingConnectionEndpoint(){
    stopWorkerThread();
}

void ReceivingConnectionEndpoint::start(int fileDescriptor){
    FD = fileDescriptor;
    spawnWorkerThread();
}
uint8_t ReceivingConnectionEndpoint::pullByte(){
    std::unique_lock lock(mutex);

    conditionVar.wait(lock, [&](){return buffer.size() >= 1;});

    uint8_t byte = buffer.front();
    buffer.pop_front();

    lock.unlock();
    conditionVar.notify_one();
    
    return byte;
}
std::deque<uint8_t> ReceivingConnectionEndpoint::pullBytes(size_t n){
    std::unique_lock lock(mutex);
    std::deque<uint8_t> bytes;
    
    conditionVar.wait(lock, [&](){return buffer.size() >= n;});

    bytes.insert(bytes.begin(), buffer.begin(), std::next(buffer.begin(), n));
    buffer.erase(buffer.begin(), std::next(buffer.begin(), n));

    lock.unlock();
    conditionVar.notify_one();

    return bytes;
}

uint8_t ReceivingConnectionEndpoint::readByte() const{
    std::unique_lock lock(mutex);

    conditionVar.wait(lock, [&](){return buffer.size() >= 1;});

    uint8_t byte = buffer.front();

    lock.unlock();
    conditionVar.notify_one();
    
    return byte;
}
std::deque<uint8_t> ReceivingConnectionEndpoint::readBytes(size_t n) const{
    std::unique_lock lock(mutex);
    std::deque<uint8_t> bytes;
    
    conditionVar.wait(lock, [&](){return buffer.size() >= n;});

    bytes.insert(bytes.begin(), buffer.begin(), std::next(buffer.begin(), n));

    lock.unlock();
    conditionVar.notify_one();

    return bytes;
}


void ReceivingConnectionEndpoint::spawnWorkerThread(){
    sendingThread = std::thread([&](){
        std::unique_lock lock(mutex);

        std::array<uint8_t, 1024> intermediateBuffer;

        while (!stopSending){
            conditionVar.wait(lock, [&](){return buffer.size() <= 1 && !stopSending;});
            if (stopSending) break;

            size_t availableBytes = std::min(buffer.size(), intermediateBuffer.size());

            std::copy(buffer.begin(), std::next(buffer.begin(), availableBytes), intermediateBuffer.begin());

            int sentBytes = send(FD, intermediateBuffer.data(), availableBytes, 0);
            buffer.erase(buffer.begin(), std::next(buffer.begin(), sentBytes));
        }
    });
}

void ReceivingConnectionEndpoint::stopWorkerThread(){
    stopSending = true;
    conditionVar.notify_one();
    if (sendingThread.joinable())
        sendingThread.join();
}

}