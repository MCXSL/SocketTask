#pragma once

class SocketServer
{
public:
    explicit SocketServer(int port = 8080);
    ~SocketServer();

    SocketServer(const SocketServer&) = delete;
    SocketServer& operator=(const SocketServer&) = delete;

    bool start();
    bool waitClient();
    int receive();
    void disconnect() noexcept;

private:
    void closeServerSocket() noexcept;

    int serverSocket_ = -1;
    int clientSocket_ = -1;
    int port_;
};
