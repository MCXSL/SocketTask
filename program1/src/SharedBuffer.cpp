//
// Created by NoName on 24.07.2026.
//

#include "../include/SharedBuffer.h"

#include <mutex>
#include <string>



void SharedBuffer::put(const std::string& str)
{
    std::unique_lock<std::mutex> lock(mutex_);

    canPut_.wait(lock, [this]()
    {
        return !hasData_;
    });

    data_ = str;
    hasData_ = true;

    canGet_.notify_one();
}

std::string SharedBuffer::get()
{
    std::unique_lock<std::mutex> lock(mutex_);

    canGet_.wait(lock, [this]()
    {
        return hasData_;
    });

    std::string result = data_;

    data_.clear();
    hasData_ = false;

    canPut_.notify_one();

    return result;
}
