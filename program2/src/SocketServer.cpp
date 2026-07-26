#include "SocketServer.h"

#include "Logger.h"

#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace
{
    constexpr auto acceptRetryDelay = std::chrono::seconds(1);
}

SocketServer::SocketServer(int port)
    : port_(port)
{
}

SocketServer::~SocketServer()
{
    disconnect();
    closeServerSocket();
}

bool SocketServer::start()
{
    disconnect();
    closeServerSocket();

    serverSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        Logger::log("Failed to create server socket");
        return false;
    }

    const int reuseAddress = 1;
    if (::setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR,
                     &reuseAddress, sizeof(reuseAddress)) < 0) {
        Logger::log("Failed to configure server socket");
        closeServerSocket();
        return false;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<std::uint16_t>(port_));
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (::bind(serverSocket_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        Logger::log("Failed to bind to port " + std::to_string(port_));
        closeServerSocket();
        return false;
    }

    if (::listen(serverSocket_, 1) < 0) {
        Logger::log("Failed to listen on port " + std::to_string(port_));
        closeServerSocket();
        return false;
    }

    Logger::log("Server started on port " + std::to_string(port_));
    return true;
}

bool SocketServer::waitClient()
{
    if (serverSocket_ == -1) {
        return false;
    }

    Logger::log("Waiting for client...");
    do {
        clientSocket_ = ::accept(serverSocket_, nullptr, nullptr);
    } while (clientSocket_ < 0 && errno == EINTR);

    if (clientSocket_ < 0) {
        Logger::log("Failed to accept client connection: " + std::to_string(errno));
        return false;
    }

    Logger::log("Client connected");
    return true;
}

int SocketServer::receive()
{
    while (true) {
        std::uint32_t payload = 0;
        auto* data = reinterpret_cast<char*>(&payload);
        std::size_t received = 0;

        while (received < sizeof(payload)) {
            const auto result = ::recv(
                clientSocket_, data + received, sizeof(payload) - received, 0);

            if (result > 0) {
                received += static_cast<std::size_t>(result);
                continue;
            }
            if (result < 0 && errno == EINTR) {
                continue;
            }

            if (result == 0) {
                Logger::log("Client closed connection");
            } else {
                Logger::log("Receive error: " + std::to_string(errno));
            }
            break;
        }

        if (received == sizeof(payload)) {
            const int value = static_cast<int>(ntohl(payload));
            Logger::log("Received value: " + std::to_string(value));
            return value;
        }

        disconnect();
        while (!waitClient()) {
            std::this_thread::sleep_for(acceptRetryDelay);
        }
    }
}

void SocketServer::disconnect() noexcept
{
    if (clientSocket_ != -1) {
        ::close(clientSocket_);
        clientSocket_ = -1;
        Logger::log("Client disconnected");
    }
}

void SocketServer::closeServerSocket() noexcept
{
    if (serverSocket_ != -1) {
        ::close(serverSocket_);
        serverSocket_ = -1;
        Logger::log("Server socket closed");
    }
}
