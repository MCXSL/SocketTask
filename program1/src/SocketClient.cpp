//
// Created by NoName on 25.07.2026.
//

#include "../include/SocketClient.h"

#include <arpa/inet.h>
#include <unistd.h>

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
    socket_ = socket(AF_INET,
                     SOCK_STREAM,
                     0);

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
        return false;

    return true;
}

bool SocketClient::sendValue(int value)
{
    return send(socket_,
                &value,
                sizeof(value),
                0) == sizeof(value);
}
