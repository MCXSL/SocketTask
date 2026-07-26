#include "SharedBuffer.h"

#include <utility>

bool SharedBuffer::put(std::string value)
{
    {
        std::lock_guard lock(mutex_);
        if (closed_) {
            return false;
        }
        data_.push(std::move(value));
    }

    dataAvailable_.notify_one();
    return true;
}


SharedBuffer::ReadStatus SharedBuffer::getFor(
    std::string& value, std::chrono::milliseconds timeout)
{
    std::unique_lock lock(mutex_);
    const bool ready = dataAvailable_.wait_for(lock, timeout, [this] {
        return closed_ || !data_.empty();
    });

    if (!ready) {
        return ReadStatus::Timeout;
    }
    if (data_.empty()) {
        return ReadStatus::Closed;
    }

    value = std::move(data_.front());
    data_.pop();
    return ReadStatus::Value;
}

void SharedBuffer::close()
{
    {
        std::lock_guard lock(mutex_);
        closed_ = true;
    }
    dataAvailable_.notify_all();
}

