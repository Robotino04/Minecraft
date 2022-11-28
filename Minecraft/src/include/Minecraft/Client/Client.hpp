#pragma once

#include <string>
#include <netdb.h>
#include <vector>
#include <functional>

#include "Minecraft/SendingConnectionEndpoint.hpp"
#include "Minecraft/ReceivingConnectionEndpoint.hpp"
#include "Minecraft/DataSink.hpp"
#include "Minecraft/DataSource.hpp"
namespace Minecraft::Client{

class Client{
    public:
        /**
         * @brief Create client.
         *
         * Creates a client using given hostname and port.
         * No connection will be made until connect is called.
         */
        Client(std::string const& host, int port=25565);

        /**
         * @brief Destroy the Client object and disconnect.
         * 
         */
        ~Client();

        /**
         * @brief Connects the socket to the server.
         * 
         * This will not send a handshake nor
         * any other package.
         * 
         * @returns whether connecting succeeded
         */
        bool connect();

        /**
         * @brief Disconnect from server.
         * 
         * Disconnects the socket from the server.
         * 
         * @returns whether disconnecting succeeded
        */
        bool disconnect();

        /**
         * @brief Get the Sink object.
         * 
         * @return SendingConnectionEndpoint& sink for sending data to the server
         */
        SendingConnectionEndpoint& getSink();
        
        /**
         * @brief Get the Source object.
         * 
         * @return ReceivingConnectionEndpoint& source for recieving data from the server
         */
        ReceivingConnectionEndpoint& getSource();
        

    private:
        std::string host;
        int port;
        bool connected = false;

        int serverFD;
        ReceivingConnectionEndpoint recvEnd;
        SendingConnectionEndpoint sendEnd; 
};

}