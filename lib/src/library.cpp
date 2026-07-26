#include "library.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <string>
#include <utility>

namespace lib
{
    namespace
    {
        bool isEvenDigit(char value)
        {
            const auto character = static_cast<unsigned char>(value);
            return std::isdigit(character) && (value - '0') % 2 == 0;
        }
    }

    void processString(std::string& str)
    {
        std::ranges::sort(str, std::greater{});

        std::string result;
        result.reserve(str.size() * 2);
        for (const char value : str) {
            if (isEvenDigit(value)) {
                result += "KB";
            } else {
                result += value;
            }
        }

        str = std::move(result);
    }

    int calculateSum(const std::string& str)
    {
        int sum = 0;
        for (const char value : str) {
            if (std::isdigit(static_cast<unsigned char>(value))) {
                sum += value - '0';
            }
        }
        return sum;
    }

    bool checkSum(int sum)
    {
        return sum >= 100 && sum % 32 == 0;
    }
}
