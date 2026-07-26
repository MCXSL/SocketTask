#pragma once

#include <fstream>
#include <mutex>
#include <string>

class Logger final
{
public:
    static bool configure(const std::string& fileName) noexcept;
    static void log(const std::string& message) noexcept;

private:
    Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& instance();
    static std::string formatTimestamp();

    std::mutex mutex_;
    std::ofstream file_;
};
