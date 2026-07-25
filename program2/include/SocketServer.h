//
// Created by NoName on 25.07.2026.
//

#ifndef SOCKETTASK_SOCKETSERVER_H
#define SOCKETTASK_SOCKETSERVER_H


#pragma once

class SocketServer
{
public:
    SocketServer(int port = 8080);
    ~SocketServer();

    bool waitClient();
    bool start();
    int receive();
    void disconnect();

private:
    int serverSocket_;
    int clientSocket_;
    int port_;
};

#endif //SOCKETTASK_SOCKETSERVER_H
