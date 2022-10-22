#pragma once

namespace Minecraft{

enum class ConnectionState{
    Handshaking,
    Status = 1,
    Login = 2,
    Play,
};

}