#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_set.hpp>

#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/core.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>

#include <oneapi/tbb/parallel_for.h>

#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <source_location>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct Range
{
    int64_t from{0}, to{0};
};
using Ranges = std::vector<Range>;

struct Mapping
{
    int64_t src {0};
    int64_t dest{0};
    int64_t size{0};
};

using Mappings = std::vector<Mapping>;

struct Data {
    std::vector<int64_t> seeds;
    std::vector<Mappings> maps;

    int64_t translate(int64_t seed) const noexcept
    {
        for (auto const& a_map : maps) {
            seed = translate(seed, a_map);
        }
        return seed;
    }

    int64_t translate(int64_t seed, Mappings const& mappings) const noexcept
    {
        for (auto const& m: mappings) {
            if (seed >= m.src && seed < (m.src + m.size)) {
                seed = (seed - m.src) + m.dest;
                break;
            }
        }
        return seed;
    }
};

void part1(Data const& data)
{
    int64_t lowest {std::numeric_limits<int64_t>::max()};
    for (int64_t seed : data.seeds) {
        seed = data.translate(seed);
        lowest = std::min(lowest, seed);
    }

    fmt::print("1: {}\n", lowest);
}

void part2_tbb(Data const& data)
{
    std::atomic<int64_t> lowest {std::numeric_limits<int64_t>::max()};
    for (auto it = data.seeds.begin(); it != data.seeds.end();) {
        const int64_t seed {*it++};
        const int64_t length {*it++};
        oneapi::tbb::parallel_for(seed, seed + length, [&lowest, &data](int64_t _seed) {
            _seed = data.translate(_seed);
            int64_t prev = lowest.load();
            while (prev > _seed && !lowest.compare_exchange_weak(prev, _seed));
        });
    }

    fmt::print("2: {}\n", lowest.load());
}

void dump(Ranges const& rr, std::string const& str)
{
    fmt::print("{}: [", str);
    for (Range const& r : rr) {
        fmt::print(",({}, {})", r.from, r.to);
    }
    fmt::print("]\n");
    fflush(stdout);
}

void part2_icl(Data const& data)
{
    using Interval = boost::icl::interval<int64_t>;
    using ISet = boost::icl::interval_set<int64_t>;

    ISet r1;

    // auto dump = [](ISet const& r, std::string const& name, std::source_location src = std::source_location::current()) {
    //     std::stringstream ss;
    //     ss << r;
    //     fmt::print("{}: {}\n", name, ss.str());
    // };

    for (auto it = data.seeds.begin(); it != data.seeds.end();) {
        const int64_t seed {*it++};
        const int64_t length {*it++};
        r1 += Interval::closed(seed, seed + length);
    }

    for (auto const& a_map : data.maps) {
        ISet rx;
        for (auto const& m : a_map) {
            auto i1 {Interval::right_open(m.src, m.src + m.size)};
            const int64_t diff = m.dest - m.src;
            auto r2 = r1 & ISet{i1};
            for (auto i2 : r2) {
                r1 -= i2;
                rx += Interval::right_open(i2.lower() + diff, i2.upper() + diff);
            }
        }
        r1 += rx;
    }

    fmt::print("2: {}\n", r1.begin()->lower());
}

int main()
{
    Data data;

    auto parse_map = []() -> Mappings {
        Mappings mappings;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) break;
            std::vector<std::string> parts;
            boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
            assert(parts.size() == 3);
            mappings.push_back(
                {.src = std::stoll(parts.at(1)), .dest = std::stoll(parts.at(0)), .size = std::stoll(parts.at(2))});
        }
        return mappings;
    };

    std::string line;
    if (std::getline(std::cin, line) && line.starts_with("seeds:"))
    {
        using namespace ranges;
        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
        data.seeds = parts | views::drop(1) | views::transform([](auto&& v) { return std::stoll(v); })
            | to<std::vector<int64_t>>;
        std::getline(std::cin, line); // empty line
    }

    if (std::getline(std::cin, line) && line == "seed-to-soil map:") {
        data.maps.push_back(parse_map());
    }

    if (std::getline(std::cin, line) && line == "soil-to-fertilizer map:") {
        data.maps.push_back(parse_map());
    }

    if (std::getline(std::cin, line) && line == "fertilizer-to-water map:") {
        data.maps.push_back(parse_map());
    }

    if (std::getline(std::cin, line) && line == "water-to-light map:") {
        data.maps.push_back(parse_map());
    }

    if (std::getline(std::cin, line) && line == "light-to-temperature map:") {
        data.maps.push_back(parse_map());
    }

    if (std::getline(std::cin, line) && line == "temperature-to-humidity map:") {
        data.maps.push_back(parse_map());
    }

    if (std::getline(std::cin, line) && line == "humidity-to-location map:") {
        data.maps.push_back(parse_map());
    }

    assert(data.maps.size() == 7);

    part1(data);
    part2_icl(data);

    return 0;
}
