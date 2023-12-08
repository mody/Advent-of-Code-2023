#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <unordered_map>
#include <vector>

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

    return 0;
}
