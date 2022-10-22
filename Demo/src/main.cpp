#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <thread>

#include <strings.h>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Minecraft/Client/Client.hpp"

#include "Minecraft/PacketID.hpp"
#include "Minecraft/PacketCoder.hpp"
#include "Minecraft/ConnectionState.hpp"

void handleUnknownPacket(Minecraft::PacketID id, size_t length){
    std::cout << "Unknown packet with ID: 0x"
                << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(id)
                << " Length: 0x"
                << length
                << std::setw(0) << std::dec
                << "\n";
}

int main(int argc, const char** argv){
    /*
    C→S: Handshake with Next State set to 2 (login)
    C→S: Login Start
    S→C: Encryption Request
    Client auth
    C→S: Encryption Response
    Server auth, both enable encryption
    S→C: Set Compression (optional)
    S→C: Login Success
    */

    const std::string host = "127.0.0.1";
    const uint16_t port = 25565;

    Minecraft::Client::Client client(host, port);
    Minecraft::ConnectionState state = Minecraft::ConnectionState::Handshaking;

    if (!client.connect()){
        std::cout << "Connecting failed!\n";
        return 1;
    }
    std::cout << "connected\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    // C→S: Handshake with Next State set to 2 (login)
    {
        Minecraft::PacketCoder encoder;
        encoder << (int32_t) 760
                << host
                << port
                << static_cast<int32_t>(Minecraft::ConnectionState::Login);

        if (!client.sendData(encoder.toPacket(Minecraft::PacketID::Handshake))){
            std::cout << "sending handshakePacket failed!\n";
        }
        else{
            std::cout << "sending handshakePacket succeeded!\n";
            state = Minecraft::ConnectionState::Login;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // C→S: Login Start
    {
        Minecraft::PacketCoder encoder;
        encoder << std::string("MinecraftBot")
                << false
                << false;

        if (!client.sendData(encoder.toPacket(Minecraft::PacketID::Handshake))){
            std::cout << "sending loginStartPacket failed!\n";
        }
        else{
            std::cout << "sending loginStartPacket succeeded!\n";
        }
    }


    // S→C: Login Success

    Minecraft::PacketCoder decoder;

    // start another thread to continuously read the incoming data
    bool dataRecieveThreadRunning = true; 
    std::thread dataRecieveThread([&](){
        while (dataRecieveThreadRunning){
            decoder << client.readData();
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    while (true){
        int32_t idInt;

        int32_t length;
        Minecraft::PacketID id;


        decoder >> length;
        decoder.getDecodedBytes();
        decoder >> idInt;

        size_t dataLength = length - decoder.getDecodedBytes();

        decoder.ignore(dataLength);
        id = static_cast<Minecraft::PacketID>(idInt);

        switch(id){
            case Minecraft::PacketID::LoginSuccess:{
                if (state == Minecraft::ConnectionState::Login){
                    std::cout << "Login successfull!\n";
                    state = Minecraft::ConnectionState::Play;
                }
                else
                    handleUnknownPacket(id, length);
                break;
            }

            default:{
                handleUnknownPacket(id, length);
                break;
            }
        }
    }

    dataRecieveThreadRunning = false;
    dataRecieveThread.join();
    

    if (!client.disconnect()){
        std::cout << "Disconnecting failed!\n";
        return 1;
    }
    std::cout << "disconnected\n";

    return 0;
}