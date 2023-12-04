#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/view/subrange.hpp>
#include <range/v3/view/all.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main()
{
    unsigned sum1{0};

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        std::vector<std::string> cards;
        boost::algorithm::split(cards, line, boost::algorithm::is_any_of(" :"), boost::token_compress_on);

        auto my_from{std::next(cards.begin(), 2)};
        auto my_to{std::find(cards.begin(), cards.end(), "|")};
        assert(my_to != cards.end());

        auto from{std::next(my_to)};
        auto to{cards.end()};

        std::sort(my_from, my_to, [](std::string const& lhs, std::string const& rhs) {
            return std::stoul(lhs) < std::stoul(rhs);
        });

        std::sort(from, to, [](std::string const& lhs, std::string const& rhs) {
            return std::stoul(lhs) < std::stoul(rhs);
        });

        std::vector<std::string> result;
        std::set_intersection(
            my_from, my_to, from, to, std::back_inserter(result),
            [](std::string const& lhs, std::string const& rhs) { return std::stoul(lhs) < std::stoul(rhs); });

        // fmt::print(
        //     "{} ; {} ; {}\n", ranges::make_subrange(my_from, my_to), ranges::make_subrange(from, to),
        //     ranges::view::all(result));
        // fmt::print("Card {} -> {}\n", cards.at(1), result.size());
        if (!result.empty()) {
            sum1 += std::pow(2, result.size() - 1);
        }
    }

    fmt::print("1: {}\n", sum1);

    return 0;
}
