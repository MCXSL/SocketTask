#include "Logger.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <system_error>

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

bool Logger::configure(const std::string& fileName) noexcept
{
    try {
        auto& logger = instance();
        std::lock_guard lock(logger.mutex_);

        std::error_code error;
        const std::filesystem::path logDirectory = "logs";
        std::filesystem::create_directories(logDirectory, error);
        if (error) {
            return false;
        }

        if (logger.file_.is_open()) {
            logger.file_.close();
        }
        logger.file_.clear();
        logger.file_.open(logDirectory / fileName, std::ios::app);
        return logger.file_.is_open();
    } catch (...) {
        return false;
    }
}

void Logger::log(const std::string& message) noexcept
{
    try {
        auto& logger = instance();
        std::lock_guard lock(logger.mutex_);
        if (logger.file_) {
            logger.file_ << '[' << formatTimestamp() << "] " << message << '\n';
            logger.file_.flush();
        }
    } catch (...) {
    }
}

std::string Logger::formatTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#if defined(__unix__) || defined(__APPLE__)
    localtime_r(&time, &localTime);
#else
    localtime_s(&localTime, &time);
#endif

    char buffer[20]{};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return buffer;
}
