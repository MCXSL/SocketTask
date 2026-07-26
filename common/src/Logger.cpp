//
// Created by yaroslav on 26.07.2026.
//

#include "../include/Logger.h"

#include <filesystem>


Logger::Logger() {
    try {
        std::filesystem::create_directories("logs");
        file_.open("logs/SocketServer.log", std::ios::app);
    } catch (...) {
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string &msg) {
    getInstance().file_ << "[" << formatTimestamp() << "] " << msg << std::endl;
    getInstance().file_.flush();
}

std::string Logger::formatTimestamp() {
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
    return std::string(buf);
}





