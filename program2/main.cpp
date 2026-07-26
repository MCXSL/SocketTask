#include "CommandLine.h"
#include "Logger.h"
#include "SocketServer.h"
#include "library.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    const auto port = commandLine::parsePort(argc, argv);
    if (!port) {
        std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
        return 1;
    }

    if (!Logger::configure("SocketServer.log")) {
        std::cerr << "Warning: failed to initialize server logging." << std::endl;
    }
    Logger::log("Server application starting on port " + std::to_string(*port));

    SocketServer server(*port);
    if (!server.start()) {
        std::cerr << "Failed to start the server." << std::endl;
        return 1;
    }
    if (!server.waitClient()) {
        std::cerr << "Failed to accept a client connection." << std::endl;
        return 1;
    }

    while (true) {
        const int value = server.receive();
        if (lib::checkSum(value)) {
            std::cout << "Received data: " << value << std::endl;
        } else {
            std::cout << "Error: incorrect data" << std::endl;
        }
    }
}
