#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <string>

class SharedBuffer
{
public:
    enum class ReadStatus
    {
        Value,
        Timeout,
        Closed
    };

    SharedBuffer() = default;
    SharedBuffer(const SharedBuffer&) = delete;
    SharedBuffer& operator=(const SharedBuffer&) = delete;

    bool put(std::string value);
    std::optional<std::string> get();
    ReadStatus getFor(std::string& value, std::chrono::milliseconds timeout);
    void close();
    bool isClosed() const;

private:
    mutable std::mutex mutex_;
    std::condition_variable dataAvailable_;
    std::queue<std::string> data_;
    bool closed_ = false;
};
