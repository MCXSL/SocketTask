//
// Created by NoName on 25.07.2026.
//

#include "../include/SocketClient.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

SocketClient::SocketClient(const char* ip,
                           int port)
    : socket_(-1),
      ip_(ip),
      port_(port)
{
}

SocketClient::~SocketClient()
{
    if (socket_ != -1)
        close(socket_);
}

bool SocketClient::connectServer()
{
    socket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_ < 0)
        return false;

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port_);

    inet_pton(AF_INET,
              ip_,
              &address.sin_addr);

    if (connect(socket_,
                (sockaddr*)&address,
                sizeof(address)) < 0)
    {
        close(socket_);
        socket_ = -1;
        return false;
    }

    return true;
}

bool SocketClient::reconnect()
{
    if (socket_ != -1)
    {
        close(socket_);
        socket_ = -1;
    }

    while (!connectServer())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return true;
}

bool SocketClient::tryReconnect()
{
    if (socket_ != -1)
    {
        close(socket_);
        socket_ = -1;
    }

    return connectServer();
}

bool SocketClient::sendValue(int value)
{
    if (socket_ == -1)
        return false;

    int bytes = send(socket_,
                     &value,
                     sizeof(value),
                     MSG_NOSIGNAL);

    if (bytes != sizeof(value))
    {
        close(socket_);
        socket_ = -1;
        return false;
    }

    return true;
}
