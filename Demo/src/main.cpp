#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <thread>
#include <fstream>
#include <tuple>
#include <future>
#include <algorithm>

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
#include "Minecraft/Hand.hpp"
#include "Minecraft/Face.hpp"
#include "Minecraft/Angle.hpp"

#include "Minecraft/Utils/IO.hpp"

// Packets
#include "Minecraft/Packets/Login/Serverbound/HandshakePacket.hpp"
#include "Minecraft/Packets/Login/Serverbound/LoginStartPacket.hpp"

#include "Minecraft/Packets/Play/Clientbound/KeepAlive.hpp"
#include "Minecraft/Packets/Play/Clientbound/SetHealthPacket.hpp"
#include "Minecraft/Packets/Play/Clientbound/SpawnPlayerPacket.hpp"
#include "Minecraft/Packets/Play/Clientbound/SynchronizePlayerPositionPacket.hpp"
#include "Minecraft/Packets/Play/Clientbound/TeleportEntityPacket.hpp"
#include "Minecraft/Packets/Play/Clientbound/UpdateEntityPositionAndRotationPacket.hpp"
#include "Minecraft/Packets/Play/Clientbound/UpdateEntityPositionPacket.hpp"
#include "Minecraft/Packets/Play/Clientbound/UpdateEntityRotationPacket.hpp"
#include "Minecraft/Packets/Play/Clientbound/PlayerCommandPacket.hpp"

#include "Minecraft/Packets/Play/Serverbound/ConfirmTeleportationPacket.hpp"
#include "Minecraft/Packets/Play/Serverbound/SetPlayerPositionPacket.hpp"
#include "Minecraft/Packets/Play/Serverbound/UseItemOnPacket.hpp"

void printHelp(std::string const& argv0){
    std::cout << "Usage: " << argv0 << " [options]\n";
    std::cout << "Options:\n\t"
              << "--log-packets:\t\t Log incoming packets. \n\t"
              << "--log-data:\t\t Log packet data. !!This option prints a lot of data to the terminal and may slow down significantly!!\n\t"
              << "--dump-packets <file>:\t Save all packets to the file.\n\t"
              << "-p, --port <port>\t Connect to a non-default port.\n\t"
              << "-H, --host <host>\t Connect to the given host.\n\t"
              << "-h, --help\t\t Print this help.\n";
}
void printMissingArgument(std::string const& required, std::string const& arg, std::string const& argv0){
    std::cout << "No " << required << " given after \"" << arg << "\"!\nSee " << argv0 << " --help for usage.\n";
}


struct PlayerState{
    double x, y, z;
    bool onGround = true;
    float health;
    Minecraft::VarInt food;
    float foodSaturation;
    Minecraft::VarInt entityID;
};


int main(int argc, const char** argv){
    bool doLogData = false;
    bool doDumpPackets = false;
    bool doLogPackets = false;
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
                doLogPackets = true;
                doLogData = true;
            }
            else if (arg == "--log-packets"){
                doLogPackets = true;
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


    Minecraft::Client::Client client;

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
    PlayerState player{
        .onGround = false,
    };
    std::vector<Minecraft::SpawnPlayerPacket> otherPlayers;

    std::cout << "Waiting for keypress before connecting...\n";
    std::cin.get();

    if (!client.connect(host, port)){
        std::cout << "Connecting failed!\n";
        return 1;
    }
    std::cout << "connected\n";


    // C→S: Handshake with Next State set to 2 (login)
    {
        Minecraft::HandshakePacket packet{
            .protocolVersion = 760,
            .host = host,
            .port = port,
            .newConnectionState = Minecraft::ConnectionState::Login,
        };
        
        Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::Handshake, packet);
        state = Minecraft::ConnectionState::Login;
    }

    // C→S: Login Start
    {
        Minecraft::LoginStartPacket packet;
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
        while ((std::cin >> word, word) != "end"){
            if (word[0] == 'x' || word[0] == 'y' || word[0] == 'z'){
                char direction = word[0];
                word.erase(word.begin());
                double offset = std::stod(word);
                switch(direction){
                    case 'x': player.x += offset; break;
                    case 'y': player.y += offset; break;
                    case 'z': player.z += offset; break;
                }

                Minecraft::SetPlayerPositionPacket packet{
                    .x = player.x,
                    .y = player.y,
                    .z = player.z,
                    .onGround = player.onGround,
                };

                Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::SetPlayerPosition, packet);
            }

            else if (word == "place"){
                Minecraft::UseItemOnPacket packet{
                    .hand = Minecraft::Hand::MainHand,
                    .face = Minecraft::Face::Top,
                    .cursorPositionX = 0.5,
                    .cursorPositionY = 1,
                    .cursorPositionZ = 0.5,
                    .insideBlock = false,
                    .sequence = 1,
                };

                std::cin >> packet.location.x;
                std::cin >> packet.location.y;
                std::cin >> packet.location.z;

                Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::UseItemOn, packet);
                std::cout << "Placed Block\n";
            }
            else if (word == "hide"){
                Minecraft::UseItemOnPacket packet{
                    .hand = Minecraft::Hand::MainHand,
                    .face = Minecraft::Face::Top,
                    .cursorPositionX = 0.5,
                    .cursorPositionY = 1,
                    .cursorPositionZ = 0.5,
                    .insideBlock = false,
                    .sequence = 1,
                };

                for (int y=-1; y<=2;y++){
                    for (int x=-1; x<=1;x++){
                        for (int z=-1; z<=1;z++){
                            if (x == 0 && z == 0 && (y == 0 || y == 1)) continue;

                            packet.location.x = player.x + x - .5;
                            packet.location.y = player.y + y;
                            packet.location.z = player.z + z - .5;

                            Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::UseItemOn, packet);
                        }
                    }
                }
                std::cout << "Hidden\n";
            }
            else if (word == "sneak"){
                Minecraft::PlayerCommandPacket packet{
                    .entityID = player.entityID,
                    .actionID = Minecraft::PlayerCommandAction::startSneaking,
                    .jumpBoost = 0,
                };

                Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::PlayerCommand, packet);
            }
            else if (word == "unsneak"){
                Minecraft::PlayerCommandPacket packet{
                    .entityID = player.entityID,
                    .actionID = Minecraft::PlayerCommandAction::stopSneaking,
                    .jumpBoost = 0,
                };

                Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::PlayerCommand, packet);
            }
            else if (word == "sprint"){
                Minecraft::PlayerCommandPacket packet{
                    .entityID = player.entityID,
                    .actionID = Minecraft::PlayerCommandAction::startSprinting,
                    .jumpBoost = 0,
                };

                Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::PlayerCommand, packet);
            }
            else if (word == "unsprint"){
                Minecraft::PlayerCommandPacket packet{
                    .entityID = player.entityID,
                    .actionID = Minecraft::PlayerCommandAction::stopSprinting,
                    .jumpBoost = 0,
                };

                Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::PlayerCommand, packet);
            }
        };
        running = false;
    });



    // S→C: Login Success
    while (running){
        Minecraft::PacketCoder::decode(client.getSource(), length);
        dataLength = length - Minecraft::PacketCoder::decode(client.getSource(), idInt);
        id = static_cast<Minecraft::PacketID>(idInt.value);

        switch(id){
            case Minecraft::PacketID::LoginSuccess:{
                if (state == Minecraft::ConnectionState::Login){ // Minecraft::PacketID::LoginSuccess
                    std::cout << "Login successfull!\n";
                    state = Minecraft::ConnectionState::Play;
                }
                else if (state == Minecraft::ConnectionState::Play){ // Minecraft::PacketID::SpawnPlayer
                    Minecraft::SpawnPlayerPacket& packet = otherPlayers.emplace_back();
                    dataLength -= Minecraft::PacketCoder::decode(client.getSource(), packet);

                    std::cout << "New player at " << Minecraft::Utils::formatPositioned(packet) << "\n";
                }
                break;
            }
            case Minecraft::PacketID::Login:{
                if (state == Minecraft::ConnectionState::Play){ // Minecraft::PacketID::SpawnPlayer
                    dataLength -= Minecraft::PacketCoder::decode(client.getSource(), player.entityID.value);
                    std::cout << "Player Entity ID: 0x" << std::hex << std::uppercase << player.entityID << std::nouppercase << std::dec << "\n";
                }
                break;
            }
            case Minecraft::PacketID::KeepAlive_Play_Client:{
                if (state == Minecraft::ConnectionState::Play){
                    Minecraft::KeepAlivePacket packet;
                    dataLength -= Minecraft::PacketCoder::decode(client.getSource(), packet);

                    // respond with the same contents
                    Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::KeepAlive_Play_Server, packet);
                }
                break;
            }
            case Minecraft::PacketID::SynchronizePlayerPosition:{
                Minecraft::SynchronizePlayerPositionPacket syncPacket;
                dataLength -= Minecraft::PacketCoder::decode(client.getSource(), syncPacket);

                player.x = syncPacket.x;
                player.y = syncPacket.y;
                player.z = syncPacket.z;

                Minecraft::ConfirmTeleportationPacket confirmPacket {syncPacket.teleportID};
                Minecraft::PacketCoder::encodePacket(client.getSink(), Minecraft::PacketID::ConfirmTeleportation, confirmPacket);

                std::cout << "New position: " << Minecraft::Utils::formatPositioned(player) << "\n";
                break;
            }
            case Minecraft::PacketID::SetHealth:{
                Minecraft::SetHealthPacket packet;
                dataLength -= Minecraft::PacketCoder::decode(client.getSource(), packet);

                player.health = packet.health;
                player.food = packet.food;
                player.foodSaturation = packet.foodSaturation;

                std::cout << "New health: Health: " << player.health << " Food: " << player.food << " Saturation: " << player.foodSaturation << "\n";
                break;
            }
            case Minecraft::PacketID::UpdateEntityPosition:{
                Minecraft::UpdateEntityPositionPacket packet;
                dataLength -= Minecraft::PacketCoder::decode(client.getSource(), packet);

                auto entityIt = std::find_if(otherPlayers.begin(), otherPlayers.end(), [&](auto const& other){return packet.entityID == other.entityID;});
                if (entityIt == otherPlayers.end()){
                    std::cout << "Invalid UpdateEntityPosition packet\n";
                }
                else{
                    entityIt->x += static_cast<double>(packet.dX) / 4096.0;
                    entityIt->y += static_cast<double>(packet.dY) / 4096.0;
                    entityIt->z += static_cast<double>(packet.dZ) / 4096.0;
                    std::cout << "New entity position: " << Minecraft::Utils::formatPositioned(*entityIt) << " (P)\n";
                }
                break;
            }
            case Minecraft::PacketID::UpdateEntityPositionAndRotation:{
                Minecraft::UpdateEntityPositionAndRotationPacket packet;
                dataLength -= Minecraft::PacketCoder::decode(client.getSource(), packet);

                auto entityIt = std::find_if(otherPlayers.begin(), otherPlayers.end(), [&](auto const& other){return packet.entityID == other.entityID;});
                if (entityIt == otherPlayers.end()){
                    std::cout << "Invalid UpdateEntityPositionAndRotation packet\n";
                }
                else{
                    entityIt->x += static_cast<double>(packet.dX) / 4096.0;
                    entityIt->y += static_cast<double>(packet.dY) / 4096.0;
                    entityIt->z += static_cast<double>(packet.dZ) / 4096.0;

                    entityIt->yaw = packet.yaw;
                    entityIt->pitch = packet.pitch;

                    std::cout << "New entity position: " << Minecraft::Utils::formatPositioned(*entityIt) << " (P&R)\n";
                }
                break;
            }
            case Minecraft::PacketID::UpdateEntityRotation:{
                Minecraft::UpdateEntityRotationPacket packet;
                dataLength -= Minecraft::PacketCoder::decode(client.getSource(), packet);

                auto entityIt = std::find_if(otherPlayers.begin(), otherPlayers.end(), [&](auto const& other){return packet.entityID == other.entityID;});
                if (entityIt == otherPlayers.end()){
                    std::cout << "Invalid UpdateEntityRotation packet\n";
                }
                else{
                    entityIt->yaw = packet.yaw;
                    entityIt->pitch = packet.pitch;
                }
                break;
            }
            case Minecraft::PacketID::TeleportEntity:{
                Minecraft::TeleportEntityPacket packet;
                dataLength -= Minecraft::PacketCoder::decode(client.getSource(), packet);

                auto entityIt = std::find_if(otherPlayers.begin(), otherPlayers.end(), [&](auto const& other){return packet.entityID == other.entityID;});
                if (entityIt == otherPlayers.end()){
                    std::cout << "Invalid TeleportEntity packet\n";
                }
                else{
                    entityIt->x = packet.x;
                    entityIt->y = packet.y;
                    entityIt->z = packet.z;

                    entityIt->yaw = packet.yaw;
                    entityIt->pitch = packet.pitch;

                    std::cout << "New entity position: " << Minecraft::Utils::formatPositioned(*entityIt) << " (TP)\n";
                }
                break;
            }
        }
        auto data = client.getSource().pullBytes(dataLength);
        dataLength = 0;

        if (doLogPackets){
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
        client.getSource().pullBytes(dataLength);
    }


    if (!client.disconnect()){
        std::cout << "Disconnecting failed!\n";
        return 1;
    }
    std::cout << "disconnected\n";

    return 0;
}