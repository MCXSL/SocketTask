//
// Created by NoName on 25.07.2026.
//

#include "../include/SocketServer.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <thread>
#include <filesystem>
#include <chrono>
#include <string>
#include "../../common/include/Logger.h"


SocketServer::SocketServer(int port)
    : serverSocket_(-1),
      clientSocket_(-1),
      port_(port)
{
}

SocketServer::~SocketServer()
{
    if (clientSocket_ != -1) {
        close(clientSocket_);
        Logger::log("Client socket closed in destructor");
    }

    if (serverSocket_ != -1) {
        close(serverSocket_);
        Logger::log("Server socket closed in destructor");
    }
}

bool SocketServer::waitClient()
{
    Logger::log("Waiting for client...");

    clientSocket_ = accept(serverSocket_, nullptr, nullptr);

    if (clientSocket_ < 0) {
        Logger::log("Failed to accept client connection");
        return false;
    }

    Logger::log("Client connected");

    return true;
}

bool SocketServer::start()
{
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket_ < 0) {
        Logger::log("Failed to create server socket");
        return false;
    }

    int opt = 1;
    setsockopt(serverSocket_,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port_);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket_,
             (sockaddr*)&address,
             sizeof(address)) < 0) {
        Logger::log(std::string("Failed to bind to port ") + std::to_string(port_));
        return false;
    }

    if (listen(serverSocket_, 1) < 0) {
        Logger::log(std::string("Failed to listen on port ") + std::to_string(port_));
        return false;
    }

    Logger::log(std::string("Server started on port ") + std::to_string(port_));

    return true;
}


int SocketServer::receive()
{
    int value;

    while (true)
    {
        int bytes = recv(clientSocket_, &value, sizeof(value), 0);

        if (bytes > 0)
        {
            Logger::log(std::string("Received value: ") + std::to_string(value));
            return value;
        }

        if (bytes == 0)
        {
            Logger::log("Client closed connection");
        }
        else
        {
            Logger::log(std::string("Recv error: ") + std::to_string(errno));
        }

        disconnect();

        // Ждём нового клиента; если accept не удался — делаем паузу и повторяем
        while (!waitClient())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        // затем в цикле попытаемся снова прочитать данные
    }
}

void SocketServer::disconnect()
{
    if (clientSocket_ != -1)
    {
        close(clientSocket_);
        clientSocket_ = -1;
        Logger::log("Client disconnected");
    }
}