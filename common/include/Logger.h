//
// Created by yaroslav on 26.07.2026.
//

#ifndef SOCKETTASK_LOGGER_H
#define SOCKETTASK_LOGGER_H

#include <string>
#include <fstream>
#include <chrono>

class Logger {
public:
    static void log(const std::string &msg);

private:
    Logger();
    static Logger& getInstance();
    static std::string formatTimestamp();
    
    std::ofstream file_;
};

#endif //SOCKETTASK_LOGGER_H
