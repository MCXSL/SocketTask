#include "CommandLine.h"

#include <charconv>
#include <string_view>

namespace commandLine
{
    std::optional<int> parsePort(int argc, char* argv[])
    {
        if (argc == 1) {
            return defaultPort;
        }
        if (argc != 2) {
            return std::nullopt;
        }

        const std::string_view text = argv[1];
        int port = 0;
        const auto [end, error] = std::from_chars(
            text.data(), text.data() + text.size(), port);

        if (error != std::errc{} || end != text.data() + text.size()
            || port < 1 || port > 65535) {
            return std::nullopt;
        }

        return port;
    }
}
