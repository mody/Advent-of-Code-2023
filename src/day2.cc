#include <algorithm>
#include <fmt/core.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/view/drop.hpp>

#include <iostream>
#include <string>
#include <vector>

struct State {
    unsigned long blue{};
    unsigned long green{};
    unsigned long red{};
};

struct Game {
    unsigned id{};
    std::vector<State> states;
};

int main()
{
    unsigned sum1{}, sum2{};

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(":;"), boost::token_compress_on);

        Game g;
        g.id = std::stoi(parts.at(0).substr(5));

        for (auto const& draw : parts | ranges::views::drop(1)) {
            std::vector<std::string> parts2;
            boost::algorithm::split(
                parts2, draw, boost::algorithm::is_any_of(" ,"), boost::token_compress_on);

            State s{};
            for (unsigned pos{1}; pos < parts2.size(); pos += 2) {
                const unsigned long val{std::stoul(parts2.at(pos))};
                if (parts2.at(pos+1) == "red") {
                    s.red = val;
                } else if (parts2.at(pos+1) == "green") {
                    s.green = val;
                } else if (parts2.at(pos+1) == "blue") {
                    s.blue = val;
                }
            }
            g.states.push_back(std::move(s));
        }

        if (ranges::all_of(g.states, [](State const& s) { return s.blue <= 14 && s.green <= 13 && s.red <= 12; })) {
            sum1 += g.id;
        }

        State max {};
        for (auto const& s : g.states) {
            max.blue = std::max(max.blue, s.blue);
            max.green = std::max(max.green, s.green);
            max.red = std::max(max.red, s.red);
        }
        sum2 += max.blue * max.green * max.red;
    }

    fmt::print("1: {}\n", sum1);
    fmt::print("2: {}\n", sum2);

    return 0;
}
