#pragma once

#include "SharedBuffer.h"

#include <optional>
#include <string>

namespace io
{
    class IO final
    {
    public:
        static void inputThread(SharedBuffer& sharedBuffer);
        static void workerThread(SharedBuffer& sharedBuffer, int port = 8080);

    private:
        enum class InputStatus
        {
            Valid,
            Empty,
            TooLong,
            NotNumeric
        };

        static InputStatus validate(const std::string& value);
        static std::optional<std::string> input();
    };
}
