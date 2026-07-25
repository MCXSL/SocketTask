#include <iostream>
#include "include/SocketServer.h"


// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

int main() {
    SocketServer server;

    if (!server.start())
    {
        std::cout << "Server start error\n";
        return 1;
    }

    while (true)
    {
        if (!server.waitClient())
            continue;

        while (true)
        {
            int value = server.receive();

            if (value == -1)
                break;

            std::cout << "Receive: " << value << std::endl;
        }

        server.disconnect();
    }
}