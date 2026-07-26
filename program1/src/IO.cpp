#include "IO.h"

#include "Logger.h"
#include "SocketClient.h"
#include "library.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <utility>

namespace io
{
    namespace
    {
        const std::size_t maxInputLength = 64;
        constexpr auto reconnectDelay = std::chrono::seconds(1);
        std::mutex consoleMutex;
        std::mutex resultMutex;
        std::condition_variable resultDisplayed;
        bool isResultDisplayed = true;

        void printLine(const std::string& message)
        {
            std::lock_guard lock(consoleMutex);
            std::cout << message << std::endl;
        }

        void waitForResult()
        {
            std::unique_lock lock(resultMutex);
            resultDisplayed.wait(lock, [] {
                return isResultDisplayed;
            });
        }

        void confirmResultDisplayed()
        {
            {
                std::lock_guard lock(resultMutex);
                isResultDisplayed = true;
            }
            resultDisplayed.notify_one();
        }
    }

    IO::InputStatus IO::validate(const std::string& value)
    {
        if (value.empty()) {
            return InputStatus::Empty;
        }
        if (value.size() > maxInputLength) {
            return InputStatus::TooLong;
        }

        const bool numeric = std::ranges::all_of(value, [](char character) {
            return std::isdigit(static_cast<unsigned char>(character));
        });
        return numeric ? InputStatus::Valid : InputStatus::NotNumeric;
    }

    std::optional<std::string> IO::input()
    {
        while (std::cin) {
            {
                std::lock_guard lock(consoleMutex);
                std::cout << "Input a numeric string (up to 64 characters): " << std::flush;
            }

            std::string value;
            if (!std::getline(std::cin, value)) {
                return std::nullopt;
            }
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }

            switch (validate(value)) {
                case InputStatus::Valid:
                    return value;
                case InputStatus::Empty:
                    Logger::log("Rejected empty input");
                    printLine("The string must not be empty.");
                    break;
                case InputStatus::TooLong:
                    Logger::log("Rejected input longer than 64 characters");
                    printLine("The string must not exceed 64 characters.");
                    break;
                case InputStatus::NotNumeric:
                    Logger::log("Rejected non-numeric input");
                    printLine("The string must contain digits only.");
                    break;
            }
        }

        return std::nullopt;
    }

    void IO::inputThread(SharedBuffer& buffer)
    {
        while (const auto value = input()) {
            Logger::log("Accepted input: " + *value);
            std::string processed = *value;
            lib::processString(processed);
            Logger::log("Processed input: " + processed);

            {
                std::lock_guard lock(resultMutex);
                isResultDisplayed = false;
            }
            if (!buffer.put(std::move(processed))) {
                return;
            }
            waitForResult();
        }

        Logger::log("Input stream closed");
        buffer.close();
    }

    void IO::workerThread(SharedBuffer& buffer, int port)
    {
        SocketClient client("127.0.0.1", port);
        std::queue<int> pendingSums;

        while (true) {
            std::string value;
            const auto status = buffer.getFor(value, reconnectDelay);

            if (status == SharedBuffer::ReadStatus::Value) {
                printLine("Result: " + value);
                confirmResultDisplayed();
                const int sum = lib::calculateSum(value);
                pendingSums.push(sum);
                Logger::log("Calculated sum: " + std::to_string(sum));
            }

            while (!pendingSums.empty()) {
                if (client.sendValue(pendingSums.front())) {
                    pendingSums.pop();
                    continue;
                }

                if (!client.tryReconnect()) {
                    break;
                }
            }

            if (status == SharedBuffer::ReadStatus::Closed) {
                Logger::log("Worker thread stopped");
                return;
            }
        }
    }
}
