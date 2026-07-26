#include "SocketClient.h"

#include "Logger.h"

#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <utility>

SocketClient::SocketClient(std::string ip, int port)
    : ip_(std::move(ip)),
      port_(port)
{
}

SocketClient::~SocketClient()
{
    closeSocket();
}

bool SocketClient::connectServer()
{
    closeSocket();
    Logger::log("Connecting to " + ip_ + ':' + std::to_string(port_));

    socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {
        Logger::log("Failed to create client socket: " + std::to_string(errno));
        return false;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<std::uint16_t>(port_));

    if (::inet_pton(AF_INET, ip_.c_str(), &address.sin_addr) != 1) {
        Logger::log("Invalid server address: " + ip_);
        closeSocket();
        return false;
    }

    if (::connect(socket_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        const int connectionError = errno;
        Logger::log("Connection failed: " + std::to_string(connectionError));
        closeSocket();
        return false;
    }

    Logger::log("Connected to server");
    return true;
}

bool SocketClient::tryReconnect()
{
    return connectServer();
}

bool SocketClient::sendValue(int value)
{
    if (socket_ == -1) {
        return false;
    }

    const std::uint32_t payload = htonl(static_cast<std::uint32_t>(value));
    const auto* bytes = reinterpret_cast<const char*>(&payload);
    std::size_t sent = 0;

    while (sent < sizeof(payload)) {
        const auto result = ::send(
            socket_, bytes + sent, sizeof(payload) - sent, MSG_NOSIGNAL);

        if (result < 0 && errno == EINTR) {
            continue;
        }
        if (result <= 0) {
            if (result == 0) {
                Logger::log("Server closed the connection during send");
            } else {
                Logger::log("Send failed: " + std::to_string(errno));
            }
            closeSocket();
            return false;
        }
        sent += static_cast<std::size_t>(result);
    }

    Logger::log("Sent value: " + std::to_string(value));
    return true;
}

void SocketClient::closeSocket() noexcept
{
    if (socket_ != -1) {
        ::close(socket_);
        socket_ = -1;
        Logger::log("Client socket closed");
    }
}
