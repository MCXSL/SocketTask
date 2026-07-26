#pragma once

#include <string>

class SocketClient
{
public:
    explicit SocketClient(std::string ip = "127.0.0.1", int port = 8080);
    ~SocketClient();

    SocketClient(const SocketClient&) = delete;
    SocketClient& operator=(const SocketClient&) = delete;

    bool connectServer();
    bool reconnect();
    bool tryReconnect();
    bool sendValue(int value);

private:
    void closeSocket() noexcept;

    int socket_ = -1;
    std::string ip_;
    int port_;
};
