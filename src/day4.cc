#include <fmt/core.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/action/sort.hpp>
#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/core.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/transform.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

struct Card
{
    unsigned id{0};
    unsigned match{0};
    unsigned count{1};
    std::vector<unsigned> my, draw;
};

void part2(std::vector<Card> cards)
{
    for (auto it {cards.begin()}; it != cards.end(); ++it) {
        Card const& c = *it;
        if (c.match) {
            for (auto i2 = std::next(it); i2 != std::next(it, c.match + 1); ++i2) {
                i2->count += c.count;
            }
        }
    }

    unsigned sum2{0};
    for (auto const& c : cards) {
        sum2 += c.count;
    }

    fmt::print("2: {}\n", sum2);
}

int main()
{
    unsigned sum1{0};

    std::vector<Card> cards;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" :"), boost::token_compress_on);

        Card c {.id = static_cast<unsigned int>(std::stoul(parts.at(1))) };

        auto my_from{std::next(parts.begin(), 2)};
        auto my_to{std::find(parts.begin(), parts.end(), "|")};
        assert(my_to != parts.end());

        auto from{std::next(my_to)};
        auto to{parts.end()};

        c.my = ranges::make_subrange(my_from, my_to)
            | ranges::views::transform([](auto&& val) { return std::stoul(val); })
            | ranges::to<std::vector<unsigned>>
            | ranges::actions::sort;

        c.draw = ranges::make_subrange(from, to)
            | ranges::views::transform([](auto&& val) { return std::stoul(val); })
            | ranges::to<std::vector<unsigned>>
            | ranges::actions::sort;

        std::vector<unsigned> result;
        ranges::set_intersection(c.my, c.draw, std::back_inserter(result));

        c.match = result.size();

        if (!result.empty()) {
            sum1 += std::pow(2, result.size() - 1);
        }

        cards.push_back(std::move(c));
    }

    fmt::print("1: {}\n", sum1);

    part2(cards);

    return 0;
}
