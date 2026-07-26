//
// Created by NoName on 24.07.2026.
//

#include "../include/SharedBuffer.h"

#include <mutex>
#include <string>



void SharedBuffer::put(const std::string& str)
{
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [this]
    {
        return !hasData_;
    });
    data_ = str;
    hasData_ = true;

    cv_.notify_one();
}

std::string SharedBuffer::get()
{
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [this]()
    {
        return hasData_;
    });

    std::string result = data_;

    data_.clear();
    hasData_ = false;
    cv_.notify_one();

    return result;
}
