#pragma once

#include <string>
#include <netdb.h>
#include <vector>

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
         * @brief Destroy the Client object and disconnect
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
         * @brief Disconnect from server
         * 
         * Disconnects the socket from the server
         * and logs off if this client was logged
         * on to the server.
         * 
         * @returns whether disconnecting succeeded
        */
        bool disconnect();

        /**
         * @brief Send data to server
         * 
         * Sends the bytes in the vector.
         * 
         * @param data the bytes to send
         * @return whether sending succeeded
         */
        bool sendData(std::vector<uint8_t> const& data);

        /**
         * @brief Read data from server
         * 
         * @param numBytes the maximum number of bytes to read
         * @return std::vector<uint8_t> 
         */
        std::vector<uint8_t> readData(size_t numBytes = 1024);
        

    private:
        std::string host;
        int port;
        bool connected = false;

        int serverFD;
};

}