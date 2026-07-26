//
// Created by NoName on 25.07.2026.
//

#ifndef SOCKETTASK_SOCKETCLIENT_H
#define SOCKETTASK_SOCKETCLIENT_H


#pragma once

class SocketClient
{
public:
    SocketClient(const char* ip = "127.0.0.1",
                 int port = 8080);

    ~SocketClient();

    bool connectServer();
    bool reconnect();
    bool tryReconnect();

    bool sendValue(int value);

private:
    int socket_;
    const char* ip_;
    int port_;
};

#endif //SOCKETTASK_SOCKETCLIENT_H
