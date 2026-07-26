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
#include "library.h"

// флаги:
// o - корректный ввод
// h - строка превышает лимит
// l - строка пуста
// w - в строке не только цифры

namespace io {
    // Общий мьютекс для защиты вывода в консоль
    static std::mutex coutMutex;

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
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << std::this_thread::get_id() << " Input string (size dont overflow 64 symbols):";
        }
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
            std::string str = input();

            lib::processString(str);
            buffer.put(str);
        }
    }

    void IO::workerThread(SharedBuffer& buffer) {
        SocketClient client;
        client.reconnect();
        while (true) {
            std::string str = buffer.get();
            
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << std::this_thread::get_id() << " Result:" << str << std::endl;
            }

            int sum = lib::calculateSum(str);

            // Не держим мьютекс вывода во время сетевых операций
            if (!client.sendValue(sum))
            {
                client.reconnect();
                client.sendValue(sum);
            }
        }
    }

} // io