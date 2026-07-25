//
// Created by NoName on 21.07.2026.
//

#include "../include/IO.h"
#include "../include/SharedBuffer.h"

#include <iostream>
#include <string>
#include <thread>
#include "library.h"

// флаги:
// o - корректный ввод
// h - строка превышает лимит
// l - строка пуста
// w - в строке не только цифры

namespace io {
    // Инициализация статических членов класса
    std::mutex IO::output_mutex_;
    std::condition_variable IO::output_cv_;
    bool IO::result_ready_ = true;  // Истина в начале, чтобы позволить первый результат

    char IO::checkString(std::string& str) {
        if (str.length() > 64) {
            return 'h';
        }

        if (str.length() <= 0 && str.empty()) {
            return 'l';
        }
        for (char c : str) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                return 'w';
            }
        }
        return 'o';
    }

    std::string IO::input() {
        std::string str;
        char flag;
        std::cout << std::this_thread::get_id << " Input string (size dont overflow 64 symbols):";
        while (std::cin >> str) {
            flag = checkString(str);
            switch (flag) {
                case 'h':
                    std::cout << "You enter over 64 symbols. Please input string correct size" << std::endl;
                    break;
                case 'l':
                    std::cout << "You enter empty string. Please input string correct size" << std::endl;
                    break;
                case 'w':
                    std::cout << "In string have are no numeric symbols. Please input correct string" << std::endl;
                    break;
                case 'o':
                    return str;
            }

        }
        return "";
    }


    void IO::inputThread(SharedBuffer& buffer) {
        while (true) {
            // Ждем, пока workerThread завершит вывод результата
            std::unique_lock<std::mutex> lock(output_mutex_);
            output_cv_.wait(lock, []() { return result_ready_; });

            lock.unlock();  // Отпускаем мьютекс перед чтением
            
            std::string str = input();
            
            lib::processString(str);
            buffer.put(str);
            
            // Устанавливаем флаг, что inputThread готов
            lock.lock();
            result_ready_ = false;
            lock.unlock();
        }
    }

    void IO::workerThread(SharedBuffer& buffer) {
        while (true) {
            std::string str = buffer.get();
            
            {
                std::lock_guard<std::mutex> lock(output_mutex_);
                std::cout << std::this_thread::get_id <<" Result:" << str << std::endl;
                
                int sum = lib::calculateSum(str);
            }
            
            // Сигнализируем inputThread, что результат выведен
            {
                std::lock_guard<std::mutex> lock(output_mutex_);
                result_ready_ = true;
            }
            output_cv_.notify_all();
        }
    }

} // io