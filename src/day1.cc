#include <boost/regex.hpp>
#include <fmt/core.h>

#include <cassert>
#include <iostream>
#include <unordered_map>

namespace {

static const boost::regex RX1{"(.*?)([0-9]|one|two|three|four|five|six|seven|eight|nine)(.*)"};
static const boost::regex RX2{"(.*)([0-9]|one|two|three|four|five|six|seven|eight|nine)(.*?)"};

static const std::unordered_map<std::string, unsigned> TO_VAL{
    {"0", 0},
    {"1", 1},
    {"2", 2},
    {"3", 3},
    {"4", 4},
    {"5", 5},
    {"6", 6},
    {"7", 7},
    {"8", 8},
    {"9", 9},
    {"one", 1},
    {"two", 2},
    {"three", 3},
    {"four", 4},
    {"five", 5},
    {"six", 6},
    {"seven", 7},
    {"eight", 8},
    {"nine", 9},
};

}  // namespace

int main()
{
    auto part1 = [](std::string_view line) {
        unsigned val{0};
        auto p1 = line.find_first_of("0123456789");
        auto p2 = line.find_last_of("0123456789");

        if (p1 != std::string::npos) {
            val += 10 * (line.at(p1) - '0');
        }
        if (p2 != std::string::npos) {
            val += line.at(p2) - '0';
        }
        return val;
    };

    auto part2 = [](std::string const& line) {
        unsigned val{0};

        boost::smatch res;

        boost::regex_search(line, res, RX1);
        val += 10 * TO_VAL.at(res.str(2));

        boost::regex_search(line, res, RX2);
        val += TO_VAL.at(res.str(2));

        return val;
    };

    unsigned sum1{0}, sum2{0};

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        sum1 += part1(line);
        sum2 += part2(line);
    }

    fmt::print("1: {}\n", sum1);
    fmt::print("2: {}\n", sum2);

    return 0;
}
