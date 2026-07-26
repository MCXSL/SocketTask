//
// Created by NoName on 21.07.2026.
//

#include "../include/IO.h"
#include "../include/SharedBuffer.h"
#include "../include/SocketClient.h"

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cctype>
#include <queue>
#include <condition_variable>
#include "library.h"

// флаги:
// o - корректный ввод
// h - строка превышает лимит
// l - строка пуста
// w - в строке не только цифры

namespace io {
    // Общий мьютекс для защиты вывода в консоль
    static std::mutex coutMutex;
    static std::mutex resultMutex;
    static std::condition_variable resultCv;
    static bool resultPrinted = true;

    char IO::checkString(const std::string& str) {
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
        while (true) {
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout <<  " Input string (size dont overflow 64 symbols):";
            }

            if (!(std::cin >> str)) {
                return "";
            }

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
    }


    void IO::inputThread(SharedBuffer& buffer) {
        while (true) {
            std::string str = input();
            if (str.empty()) {
                return;
            }

            lib::processString(str);
            {
                std::lock_guard<std::mutex> lock(resultMutex);
                resultPrinted = false;
            }

            buffer.put(str);

            std::unique_lock<std::mutex> lock(resultMutex);
            resultCv.wait(lock, []()
            {
                return resultPrinted;
            });
        }
    }

    void IO::workerThread(SharedBuffer& buffer) {
        SocketClient client;
        std::queue<int> pendingSums;
        while (true) {
            std::string str = buffer.get();

            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << " Result:" << str << std::endl;
            }
            {
                std::lock_guard<std::mutex> lock(resultMutex);
                resultPrinted = true;
            }
            resultCv.notify_one();

            int sum = lib::calculateSum(str);
            pendingSums.push(sum);

            while (!pendingSums.empty()) {
                if (client.sendValue(pendingSums.front())) {
                    pendingSums.pop();
                    continue;
                }

                if (!client.tryReconnect() || !client.sendValue(pendingSums.front())) {
                    break;
                }

                pendingSums.pop();
            }
        }
    }

} // io
