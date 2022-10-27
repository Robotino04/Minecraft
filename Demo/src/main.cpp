#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <thread>
#include <fstream>

#include <strings.h>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Minecraft/Client/Client.hpp"

#include "Minecraft/PacketID.hpp"
#include "Minecraft/PacketCoder.hpp"
#include "Minecraft/ConnectionState.hpp"
#include "Minecraft/BindTarget.hpp"

void printHelp(std::string const& argv0){
    std::cout << "Usage: " << argv0 << " [options]\n";
    std::cout << "Options:\n\t"
              << "--log-data:\t\t Log packet data. !!This option prints a lot of data to the terminal and may slow down significantly!!\n\t"
              << "--dump-packets <file>:\t Save all packets to the file.\n\t"
              << "-p, --port <port>\t Connect to a non-default port.\n\t"
              << "-H, --host <host>\t Connect to the given host.\n\t"
              << "-h, --help\t\t Print this help.\n";
}

int main(int argc, const char** argv){
    bool doLogData = false;
    bool doDumpPackets = false;
    std::string dumpLocation = "data.bin";
    std::string host = "127.0.0.1";
    uint16_t port = 25565;

    {
        int i=1;
        while (i<argc){
            std::string arg = argv[i++];
            if (arg == "--log-data"){
                doLogData = true;
            }
            else if (arg == "--dump-packets"){
                doDumpPackets = true;
                if (i != argc){
                    dumpLocation = argv[i++];
                }
                else{
                    std::cout << "No file given after \"" << arg << "\"!\nSee " << argv[0] << " --help for usage.\n";
                    exit(1);
                }
            }
            else if (arg == "-p" || arg == "--port"){
                if (i != argc){
                    port = std::stoi(argv[i++]);
                }
                else{
                    std::cout << "No port given after \"" << arg << "\"!\nSee " << argv[0] << " --help for usage.\n";
                    exit(1);
                }
            }
            else if (arg == "-H" || arg == "--host"){
                if (i != argc){
                    host = argv[i++];
                }
                else{
                    std::cout << "No host given after \"" << arg << "\"!\nSee " << argv[0] << " --help for usage.\n";
                    exit(1);
                }
            }
            else if (arg == "-h" || arg == "--help"){
                printHelp(argv[0]);
                exit(0);
            }
        }
    }

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
        std::ofstream logFile;
        if (doDumpPackets) logFile.open(dumpLocation, std::ios::binary);
        while (dataRecieveThreadRunning){
            auto data = client.readData();
            
            decoder << data;
            if (doDumpPackets) logFile.write((const char*)data.data(), data.size());
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    while (true){
        int32_t idInt;

        int32_t length;
        Minecraft::PacketID id;


        decoder >> length;
        decoder.getNumDecodedBytes();
        decoder >> idInt;

        size_t dataLength = length - decoder.getNumDecodedBytes();

        auto data = decoder.getBytes(dataLength);
        decoder.ignore(dataLength);
        id = static_cast<Minecraft::PacketID>(idInt);

        std::cout << Minecraft::getPacketDescription(id, state, Minecraft::BindTarget::Client, length);
        std::cout << std::hex << std::uppercase << std::setfill('0');
        if (doLogData){
            std::cout << " ---";
            for (auto byte : data){
                std::cout << " " << std::setw(2) << static_cast<int>(byte);
            }
        std::cout << " ---";
        }
        std::cout << std::dec << std::nouppercase << std::setw(0) << "\n";

        switch(id){
            case Minecraft::PacketID::LoginSuccess:{
                if (state == Minecraft::ConnectionState::Login){
                    std::cout << "Login successfull!\n";
                    state = Minecraft::ConnectionState::Play;
                }
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