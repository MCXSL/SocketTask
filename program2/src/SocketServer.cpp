//
// Created by NoName on 25.07.2026.
//

#include "../include/SocketServer.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <string>

// Simple file-only logger placed in this translation unit. Logs are written to
// program working directory under ./logs/SocketServer.log. Only file output
// (no stdout/stderr) is used per request.
namespace {
class FileLogger {
public:
    FileLogger() {
        try {
            std::filesystem::create_directories("logs");
            file_.open("logs/SocketServer.log", std::ios::app);
        } catch (...) {
            // best-effort: if directory creation or open fails, keep logger disabled
        }
    }

    void log(const std::string &msg) {
        if (!file_.is_open()) return;
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
#if defined(__unix__) || defined(__APPLE__)
        localtime_r(&t, &tm);
#else
        localtime_s(&tm, &t);
#endif
        char buf[64];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
        file_ << "[" << buf << "] " << msg << std::endl;
        file_.flush();
    }

private:
    std::ofstream file_;
};

static FileLogger logger;
}

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
        logger.log("Client socket closed in destructor");
    }

    if (serverSocket_ != -1) {
        close(serverSocket_);
        logger.log("Server socket closed in destructor");
    }
}

bool SocketServer::waitClient()
{
    logger.log("Waiting for client...");

    clientSocket_ = accept(serverSocket_, nullptr, nullptr);

    if (clientSocket_ < 0) {
        logger.log("Failed to accept client connection");
        return false;
    }

    logger.log("Client connected");

    return true;
}

bool SocketServer::start()
{
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket_ < 0) {
        logger.log("Failed to create server socket");
        return false;
    }

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port_);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket_,
             (sockaddr*)&address,
             sizeof(address)) < 0) {
        logger.log(std::string("Failed to bind to port ") + std::to_string(port_));
        return false;
    }

    if (listen(serverSocket_, 1) < 0) {
        logger.log(std::string("Failed to listen on port ") + std::to_string(port_));
        return false;
    }

    logger.log(std::string("Server started on port ") + std::to_string(port_));

    return true;
}


int SocketServer::receive()
{
    int value;

    int bytes = recv(clientSocket_, &value, sizeof(value), 0);

    if (bytes <= 0)
    {
        disconnect();
        waitClient();      // автоматически ждём нового клиента
        logger.log("Receive failed or connection closed");
        return receive();  // снова читаем данные
    }
    logger.log(std::string("Received value: ") + std::to_string(value));

    return value;
}

void SocketServer::disconnect()
{
    if (clientSocket_ != -1)
    {
        close(clientSocket_);
        clientSocket_ = -1;
        logger.log("Client disconnected");
    }
}