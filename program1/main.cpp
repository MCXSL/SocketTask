#include "CommandLine.h"
#include "IO.h"
#include "Logger.h"
#include "SharedBuffer.h"

#include <functional>
#include <iostream>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
    const auto port = commandLine::parsePort(argc, argv);
    if (!port) {
        std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
        return 1;
    }

    if (!Logger::configure("SocketClient.log")) {
        std::cerr << "Warning: failed to initialize client logging." << std::endl;
    }
    Logger::log("Client application started on port " + std::to_string(*port));

    SharedBuffer buffer;

    std::thread inputThreadHandle{io::IO::inputThread, std::ref(buffer)};
    std::thread workerThreadHandle{io::IO::workerThread, std::ref(buffer), *port};

    inputThreadHandle.join();
    workerThreadHandle.join();
    Logger::log("Client application stopped");
    return 0;
}
