#include <fmt/core.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/view/drop.hpp>

#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

using Tree = std::unordered_map<std::string, std::pair<std::string, std::string>>;

void part1(std::string const& instructions, Tree const& tree)
{
    unsigned steps {0};
    std::string me {"AAA"};
    while(me != "ZZZ") {
        for (unsigned char c : instructions) {
            ++steps;
            auto const& node = tree.at(me);
            if (c == 'L') {
                me = node.first;
            } else {
                me = node.second;
            }
            if (me == "ZZZ") break;
        }
    }

    fmt::print("1: {}\n", steps);
}

void part2(std::string const& instructions, Tree const& tree)
{
    std::vector<std::pair<std::string, uint64_t>> ghosts, finished;

    for (auto const& [k, _] : tree) {
        if (k.back() == 'A') ghosts.push_back({k, 0});
    }

    for (;!ghosts.empty();) {
        for (unsigned char c : instructions) {
            for (auto& ghost : ghosts) {
                auto const& node = tree.at(ghost.first);
                if (c == 'L') {
                    ghost.first = node.first;
                } else {
                    ghost.first = node.second;
                }
                ++ghost.second;
            }
            for (auto it = ghosts.begin(); it != ghosts.end();) {
                if (it->first.back() == 'Z') {
                    finished.push_back(*it);
                    it = ghosts.erase(it);
                } else {
                    ++it;
                }
            }
            if (ghosts.empty()) {
                break;
            }
        }
    }

    uint64_t steps{finished.front().second};
    for (auto const& ghost : finished | ranges::views::drop(1)) {
        steps = std::lcm(steps, ghost.second);
    }
    fmt::print("2: {}\n", steps);
}

int main()
{
    std::string instructions;
    std::getline(std::cin, instructions);

    std::unordered_map<std::string, std::pair<std::string, std::string>> tree;

    std::string line;
    std::getline(std::cin, line); // skip empty line
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" =,()"), boost::token_compress_on);
        tree.insert({parts.at(0), {parts.at(1), parts.at(2)}});
    }

    part1(instructions, tree);
    part2(instructions, tree);

    return 0;
}
