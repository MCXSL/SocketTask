#include <iostream>
#include <string>
#include <thread>
#include "library.h"
#include "include/IO.h"
#include "include/SharedBuffer.h"
#include "include/SocketClient.h"
using namespace std;


int main() {
    SharedBuffer buffer;


    std::thread t1(io::IO::inputThread, std::ref(buffer));
    std::thread t2(io::IO::workerThread, std::ref(buffer));

    t1.join();
    t2.join();

    return 0;
}