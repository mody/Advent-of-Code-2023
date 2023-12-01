#include <fmt/core.h>

#include <iostream>
#include <vector>

namespace {

constexpr unsigned to_val(char c) noexcept {
    return c - '0';
}

}  // namespace

int main()
{
    unsigned sum1{0};

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        auto p1 = line.find_first_of("0123456789");
        auto p2 = line.find_last_of("0123456789");

        if (p1 != std::string::npos) {
            sum1 += 10 * to_val(line.at(p1));
        }
        if (p2 != std::string::npos) {
            sum1 += to_val(line.at(p2));
        }
    }

    fmt::print("1: {}\n", sum1);

    return 0;
}
