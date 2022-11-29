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
         */
        Client();

        /**
         * @brief Destroy the Client object and disconnect.
         * 
         */
        ~Client();

        /**
         * @brief Connect to the server.
         * 
         * This will not send a handshake nor
         * any other package.
         * 
         * @param host the server hostname
         * @param port the server port
         * @returns whether connecting succeeded
         */

        bool connect(std::string const& host, int port=25565);

        /**
         * @brief Disconnect from server.
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
        bool connected = false;

        int serverFD;
        ReceivingConnectionEndpoint recvEnd;
        SendingConnectionEndpoint sendEnd; 
};

}