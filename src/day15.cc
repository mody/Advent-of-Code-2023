#include <deque>
#include <fmt/core.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/view/enumerate.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <string_view>
#include <vector>

using StringVec = std::vector<std::string>;

struct Item
{
    std::string id;
    unsigned value {0u};
};

using Box = std::deque<Item>;
using Pipeline = std::map<unsigned, Box>;


unsigned hash(std::string_view s) noexcept
{
    unsigned seed {0u};
    for (auto c : s) {
        seed += c;
        seed *= 17;
        seed &= 0xff;
    }
    return seed;
}

void part1(StringVec const& input)
{
    fmt::print("1: {}\n", std::accumulate(input.begin(), input.end(), 0u, [](unsigned val, std::string const& l) {
                   return val + hash(l);
               }));
}

void part2(StringVec const& input)
{
    Pipeline pipeline;

    for (auto const& line : input)
    {
        std::string_view vcmd{line};
        const auto cmd {vcmd.find_first_of("=-")};
        assert(cmd != std::string::npos);
        auto name {vcmd.substr(0, cmd)};

        Box& box = pipeline[hash(name)];
        auto it = std::find_if(box.begin(), box.end(), [&name](Item const& item) { return item.id == name; });

        if (vcmd.at(cmd) == '-') {
            if (it != box.end()) {
                box.erase(it);
            }
        } else {
            const unsigned lens = std::stoul(line.substr(cmd+1));
            if (it == box.end()) {
                box.push_back(Item{std::string{name}, lens});
            } else {
                it->value = lens;
            }
        }
    }

    unsigned sum{0u};
    for (auto const& [box_id, box] : pipeline) {
        for (auto const& [idx, item] : box | ranges::views::enumerate) {
            unsigned val = (box_id + 1) * (idx + 1u) * item.value;
            sum += val;
        }
    }

    fmt::print("2: {}\n", sum);
}

int main()
{
    StringVec input;
    {
        std::string line;
        std::getline(std::cin, line);
        boost::algorithm::split(input, line, boost::algorithm::is_any_of(","), boost::token_compress_on);
    }

    part1(input);
    part2(input);

    return 0;
}
