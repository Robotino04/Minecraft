#error This file is a template and should never be directly included! Instead use "Minecraft/PacketID.hpp".
#pragma once

#include "Minecraft/ConnectionState.hpp"
#include "Minecraft/BindTarget.hpp"

#include <string>
#include <sstream>
#include <iomanip>

namespace Minecraft{

enum class PacketID{
    // Auto generated packets here{packets}
};

inline std::string getPacketDescription(PacketID id, ConnectionState state, BindTarget boundTo, size_t length){
    uint32_t packet = static_cast<uint32_t>(id) | static_cast<uint32_t>(state) << 8 | static_cast<uint32_t>(boundTo) << 16;
    switch(packet){
        default:{
            std::stringstream ss;
            ss << "Unknown packet. (ID: 0x"
                << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                << static_cast<int>(id)
                << std::dec << std::setw(0) << std::nouppercase
                << ", state: ";
            switch(state){
                case ConnectionState::Handshaking: ss << "Handshaking"; break;
                case ConnectionState::Login: ss << "Login"; break;
                case ConnectionState::Status: ss << "Status"; break;
                case ConnectionState::Play: ss << "Play"; break;
                default: ss << "unknown state(" << static_cast<int>(state) << ")";
            }
            ss  << ", bound to: ";
            if (boundTo == BindTarget::Client)
                ss << "Client";
            else
                ss << "Server";
            
            ss << ", length: 0x"
                << std::hex << std::uppercase
                << length
                << std::dec << std::nouppercase
                << ")";
            return ss.str();
        }
        // Auto generated cases here{cases}
    }
}

}