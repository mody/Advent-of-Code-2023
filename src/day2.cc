#include <algorithm>
#include <fmt/core.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/enumerate.hpp>

#include <iostream>
#include <string>
#include <vector>

// Game 10: 2 blue, 8 green, 2 red; 10 blue, 3 green; 12 blue, 1 green, 2 red; 9 green, 2 red; 3 green, 2 red, 5 blue

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
    unsigned sum1{};

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        // Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
        // Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
        // Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
        // Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
        // Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(":;"), boost::token_compress_on);

        // for (auto const& [i,v] : parts | ranges::views::enumerate) {
        //     fmt::print("{} -> {}\n", i, v);
        // }
        // fflush(stdout);

        Game g;
        g.id = std::stoi(parts.at(0).substr(5));

        for (auto const& draw : parts | ranges::views::drop(1)) {
            std::vector<std::string> parts2;
            boost::algorithm::split(
                parts2, draw, boost::algorithm::is_any_of(" ,"), boost::token_compress_on);

            // for (auto const& [i, v] : parts2 | ranges::views::enumerate) {
            //     fmt::print("{}: {} -> {}\n", draw, i, v);
            // }
            // fflush(stdout);

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
            fmt::print("g: {}\ts: {}, {}, {}\n", g.id, s.blue, s.green, s.red);
            g.states.push_back(std::move(s));
        }


        if (ranges::all_of(g.states, [](State const& s) { return s.blue <= 14 && s.green <= 13 && s.red <= 12; })) {
            sum1 += g.id;
        }
    }

    fmt::print("1: {}\n", sum1);

    return 0;
}
