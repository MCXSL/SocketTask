#include <iostream>

#include "library.h"
#include "include/SocketServer.h"


// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

int main() {
    SocketServer server;

    server.start();
    server.waitClient();

    while (true)
    {
        int value = server.receive();

        if (lib::checkSum(value)) {
            std::cout << "Received data: " << value << std::endl;
        } else {
            std::cout << "Error: incorrect data" << std::endl;
        }
    }
}
