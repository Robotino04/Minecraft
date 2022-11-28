#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <thread>
#include <fstream>
#include <tuple>
#include <future>

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
void printMissingArgument(std::string const& required, std::string const& arg, std::string const& argv0){
    std::cout << "No " << required << " given after \"" << arg << "\"!\nSee " << argv0 << " --help for usage.\n";
}


struct HandshakePacket{
    Minecraft::VarInt protocolVersion;
    std::string host;
    uint16_t port;
    Minecraft::VarInt newConnectionState;

    PACKET_CONTENTS(
        protocolVersion,
        host,
        port,
        newConnectionState
    )
};

struct LoginStartPacket{
    std::string name;
    bool hasSigData;
    Minecraft::Conditional<int64_t> timestamp = Minecraft::Conditional<int64_t>(hasSigData);
    Minecraft::Conditional<Minecraft::VarInt> publicKeyLength = Minecraft::Conditional<Minecraft::VarInt>(hasSigData);
    Minecraft::Conditional<std::vector<uint8_t>> publicKey = Minecraft::Conditional<std::vector<uint8_t>>(hasSigData);
    Minecraft::Conditional<Minecraft::VarInt> signatureLength = Minecraft::Conditional<Minecraft::VarInt>(hasSigData);
    Minecraft::Conditional<std::vector<uint8_t>> signature = Minecraft::Conditional<std::vector<uint8_t>>(hasSigData);
    bool hasPlayerUUID;
    // Minecraft::Conditional<Minecraft::UUID> playerUUID = Minecraft::Conditional<Minecraft::UUID>(hasPlayerUUID);


    PACKET_CONTENTS(
        name,
        hasSigData,
        timestamp,
        publicKeyLength,
        publicKey,
        signatureLength,
        signature,
        hasPlayerUUID
    )
};

struct KeepAlivePacket{
    int64_t keepAliveID;

    PACKET_CONTENTS(
        keepAliveID
    )
};

int main(int argc, const char** argv){
    bool doLogData = false;
    bool doDumpPackets = false;
    std::string dumpLocation = "data.bin";
    std::string host = "127.0.0.1";
    uint16_t port = 25566;

    {
        for (int i=1; i<argc; i++){
            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help"){
                printHelp(argv[0]);
                exit(0);
            }
            else if (arg == "--log-data"){
                doLogData = true;
            }
            else if (arg == "--dump-packets"){
                doDumpPackets = true;
                if (i != argc){
                    dumpLocation = argv[++i];
                }
                else{
                    printMissingArgument("file", arg, argv[0]);
                    exit(1);
                }
            }
            else if (arg == "-p" || arg == "--port"){
                if (i != argc){
                    port = std::stoi(argv[++i]);
                }
                else{
                    printMissingArgument("port", arg, argv[0]);
                    exit(1);
                }
            }
            else if (arg == "-H" || arg == "--host"){
                if (i != argc){
                    host = argv[++i];
                }
                else{
                    printMissingArgument("host", arg, argv[0]);
                    exit(1);
                }
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

    std::ofstream dumpFile;

    if (doDumpPackets){
        dumpFile.open(dumpLocation, std::ios::binary);
        client.getSource().setCallback([&](auto begin, auto end){
            for (auto byteIt = begin; byteIt != end; byteIt++){
                dumpFile.write(reinterpret_cast<char*>(&*byteIt), 1);
            }
        });
    }

    Minecraft::ConnectionState state = Minecraft::ConnectionState::Handshaking;

    std::cout << "Waiting for keypress before connecting...\n";
    std::cin.get();

    if (!client.connect()){
        std::cout << "Connecting failed!\n";
        return 1;
    }
    std::cout << "connected\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    // C→S: Handshake with Next State set to 2 (login)
    {
        HandshakePacket packet{
            .protocolVersion = 760,
            .host = host,
            .port = port,
            .newConnectionState = static_cast<int>(Minecraft::ConnectionState::Login),
        };
        
        Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::Handshake, packet);
        state = Minecraft::ConnectionState::Login;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // C→S: Login Start
    {
        LoginStartPacket packet;
        packet.name = "MinecraftBot";
        packet.hasSigData = false;
        packet.hasPlayerUUID = false;

        Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::LoginStart, packet);
    }

    std::cout << "Login initiated.\n";

    Minecraft::VarInt idInt;

    Minecraft::VarInt length;
    Minecraft::PacketID id;
    
    size_t dataLength;

    bool running = true;

    auto inputHandlerFuture = std::async(std::launch::async, [&](){
        std::string word;
        while ((std::cin >> word, word) != "end");
        running = false;
    });

    // S→C: Login Success
    while (running){
        Minecraft::PacketCoder::decode(client.getSource(), length);
        dataLength = length - Minecraft::PacketCoder::decode(client.getSource(), idInt);
        id = static_cast<Minecraft::PacketID>(idInt.value);

        switch(id){
            case Minecraft::PacketID::LoginSuccess:{
                if (state == Minecraft::ConnectionState::Login){
                    std::cout << "Login successfull!\n";
                    state = Minecraft::ConnectionState::Play;
                }
                break;
            }
            case Minecraft::PacketID::KeepAlive_Play_Client:{
                if (state == Minecraft::ConnectionState::Play){
                    KeepAlivePacket packet;
                    dataLength -= Minecraft::PacketCoder::decode(client.getSource(), packet);

                    // respond with the same contents
                    Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::KeepAlive_Play_Server, packet);
                    std::cout << ".";
                }
                break;
            }
            default:{
                auto data = client.getSource().pullBytes(dataLength);
                dataLength = 0;

                std::cout << Minecraft::getPacketDescription(id, state, Minecraft::BindTarget::Client, length);
                if (doLogData){
                    std::cout << std::hex << std::uppercase << std::setfill('0');
                    std::cout << " ---";
                    for (auto byte : data){
                        std::cout << " " << std::setw(2) << static_cast<int>(byte);
                    }
                    std::cout << " ---";
                    std::cout << std::dec << std::nouppercase << std::setw(0);
                }
                std::cout << "\n";
            }
        }
        client.getSource().pullBytes(dataLength);
    }


    if (!client.disconnect()){
        std::cout << "Disconnecting failed!\n";
        return 1;
    }
    std::cout << "disconnected\n";

    return 0;
}