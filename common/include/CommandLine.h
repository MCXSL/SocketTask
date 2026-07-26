#pragma once

#include <optional>

namespace commandLine
{
    inline constexpr int defaultPort = 8080;

    std::optional<int> parsePort(int argc, char* argv[]);
}
