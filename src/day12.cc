#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/container_hash/hash.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/transform.hpp>

#include <cassert>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string_view>
#include <vector>

using namespace ranges;

using Counts = std::vector<unsigned>;

struct Row
{
    std::string springs;
    Counts counts;
};

using Garden = std::vector<Row>;

// thanks to https://www.youtube.com/watch?v=g3Ms5e7Jdqo
uint64_t count(std::string_view springs, std::span<const unsigned> counts)
{
    static std::map<size_t, uint64_t> cache;

    if (springs.empty()) return counts.empty() ? 1 : 0;
    if (counts.empty()) return springs.find('#') == std::string::npos ? 1 : 0;

    size_t seed = 0;
    boost::hash_combine(seed, springs);
    boost::hash_combine(seed, counts);
    const auto it = cache.find(seed);
    if (it != cache.end()) {
        return it->second;
    }

    uint64_t result {0};

    const unsigned expected = counts.front();

    if (springs.starts_with('.') || springs.starts_with('?')) {
        result += count(springs.substr(1), counts);
    }

    if (springs.starts_with('#') || springs.starts_with('?')) {
        if (expected <= springs.size() && springs.substr(0, expected).find('.') == std::string::npos &&
                (expected == springs.size() || springs.at(expected) != '#')) {
            result += count(springs.substr(expected + 1), counts.subspan(1));
        }
    }

    cache[seed] = result;
    return result;
}

void part1(Garden const& garden)
{
    uint64_t sum{0};

    for (auto const& row : garden) {
        auto val = count(row.springs + ".", row.counts);
        sum += val;
    }

    fmt::print("1: {}\n", sum);
}

void part2(Garden garden)
{
    for (auto& row : garden) {
        auto springs {row.springs};
        auto counts {row.counts};
        for (unsigned i {0}; i < 4; ++i) {
            springs += '?' + row.springs;
            counts.insert(counts.end(), row.counts.begin(), row.counts.end());
        }
        std::swap(row.springs, springs);
        std::swap(row.counts, counts);
    }

    uint64_t sum{0};

    for (auto const& row : garden) {
        auto val = count(row.springs + ".", row.counts);
        sum += val;
    }

    fmt::print("1: {}\n", sum);
}

int main()
{
    Garden garden;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        Row r;

        auto split = line.find(' ');
        r.springs = line.substr(0, split);

        std::vector<std::string> parts;
        boost::algorithm::split(
            parts, line.substr(split + 1), boost::algorithm::is_any_of(","), boost::token_compress_on);
        r.counts = parts | ranges::views::transform([](auto&& v) { return std::stoll(v); })
            | ranges::to<std::vector<unsigned>>;

        garden.push_back(std::move(r));
    }

    part1(garden);
    part2(garden);

    return 0;
}
