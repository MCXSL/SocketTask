//
// Created by NoName on 24.07.2026.
//

#ifndef SOCKETTASK_SHAREDBUFFER_H
#define SOCKETTASK_SHAREDBUFFER_H
#include <string>
#include <mutex>
#include <condition_variable>


class SharedBuffer {
    public: void put(const std::string& str);
        std::string get();
    private:
        std::mutex mutex_;
        std::condition_variable cv_;
        std::string data_;
        bool hasData_ = false;
};


#endif //SOCKETTASK_SHAREDBUFFER_H
