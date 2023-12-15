#include <fmt/core.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string_view>
#include <vector>

using StringVec = std::vector<std::string>;

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

int main()
{
    StringVec input;
    {
        std::string line;
        std::getline(std::cin, line);
        boost::algorithm::split(input, line, boost::algorithm::is_any_of(","), boost::token_compress_on);
    }

    part1(input);

    return 0;
}
