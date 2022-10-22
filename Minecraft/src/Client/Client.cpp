#include "Minecraft/Client/Client.hpp"

#include <arpa/inet.h>
#include <memory.h>
#include <unistd.h>
#include <stdexcept>

namespace Minecraft::Client{

Client::Client(std::string const& host, int port){
    this->host = host;
    this->port = port;
}

Client::~Client(){
    disconnect();
}

bool Client::connect(){
    if (connected)
        return false;

    // create socket
    serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFD == -1)
        return false;
    
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;

    // convert host address
    struct in_addr ipv4addr;
    if (inet_pton(AF_INET, host.c_str(), &ipv4addr) != 1)
        return false;
    struct hostent* hp = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);
    if (hp == nullptr)
        return false;
    
    // setup server struct
    memcpy(&(serverAddress.sin_addr.s_addr), hp->h_addr, hp->h_length);
    serverAddress.sin_port = htons(port);

    // connect the socket
    if (::connect(serverFD, (const sockaddr*) &serverAddress, sizeof(serverAddress)) != 0)
        return false;
    
    connected = true;
    return true;
}

bool Client::disconnect(){
    if (!connected)
        return false;
    
    if (close(serverFD) != 0)
        return false;
    
    connected = false;
    return true;
}


bool Client::sendData(std::vector<uint8_t> const& data){
    return send(serverFD, data.data(), data.size(), 0) == data.size();
}

std::vector<uint8_t> Client::readData(size_t numBytes){
    std::vector<uint8_t> data;
    data.resize(numBytes);
    int readBytes = read(serverFD, data.data(), data.size());
    if (readBytes == -1){
        throw std::runtime_error("Reading data from server failed!");
    }

    data.resize(readBytes);
    return data;
}

}
