//
// Created by NoName on 25.07.2026.
//

#include "../include/SocketServer.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

SocketServer::SocketServer(int port)
    : serverSocket_(-1),
      clientSocket_(-1),
      port_(port)
{
}

SocketServer::~SocketServer()
{
    if (clientSocket_ != -1)
        close(clientSocket_);

    if (serverSocket_ != -1)
        close(serverSocket_);
}

bool SocketServer::waitClient()
{
    std::cout << "Waiting for client...\n";

    clientSocket_ = accept(serverSocket_, nullptr, nullptr);

    if (clientSocket_ < 0)
        return false;

    std::cout << "Client connected\n";

    return true;
}

bool SocketServer::start()
{
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket_ < 0)
        return false;

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port_);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket_,
             (sockaddr*)&address,
             sizeof(address)) < 0)
        return false;

    if (listen(serverSocket_, 1) < 0)
        return false;

    return true;
}

int SocketServer::receive()
{
    int value;

    int bytes = recv(clientSocket_,
                     &value,
                     sizeof(value),
                     0);

    if (bytes <= 0)
        return -1;

    return value;
}

void SocketServer::disconnect()
{
    if (clientSocket_ != -1)
    {
        close(clientSocket_);
        clientSocket_ = -1;
    }
}