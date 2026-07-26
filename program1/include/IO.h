//
// Created by NoName on 21.07.2026.
//

#ifndef SOCKETTASK_IO_H
#define SOCKETTASK_IO_H
#include <string>
#include <mutex>
#include <condition_variable>
#include "../include/SharedBuffer.h"

namespace io {
    class IO {
        private:
        static char checkString(const std::string& str);
        static std::string input();

        public:
        static void inputThread(SharedBuffer& sharedBuffer);
        static void workerThread(SharedBuffer& sharedBuffer);

    };
} // io

#endif //SOCKETTASK_IO_H
