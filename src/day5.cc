#include <fmt/core.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/core.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>

#include <limits>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <map>

struct Destination
{
    uint64_t dest{0};
    uint64_t size{0};
};

using Map = std::map<uint64_t, Destination>;

struct Data {
    std::vector<uint64_t> seeds;
    std::vector<Map> maps;
};


void part1(Data const& data)
{
    uint64_t lowest {std::numeric_limits<uint64_t>::max()};
    for (uint64_t seed : data.seeds) {
        for (auto const& a_map : data.maps) {
            for (auto const& [src, dst] : a_map) {
                if (seed >= src && seed < (src + dst.size)) {
                    seed = (seed - src) + dst.dest;
                    break;
                }
            }
        }
        lowest = std::min(lowest, seed);
    }

    fmt::print("1: {}\n", lowest);
}

int main()
{
    Data data;

    auto parse_map = []() {
        Map map;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) break;
            std::vector<std::string> parts;
            boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
            assert(parts.size() == 3);
            map.insert(
                {std::stoull(parts.at(1)), {.dest = std::stoull(parts.at(0)), .size = std::stoull(parts.at(2))}});
        }
        return map;
    };

    std::string line;
    if (std::getline(std::cin, line) && line.starts_with("seeds:"))
    {
        using namespace ranges;
        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
        data.seeds = parts | views::drop(1) | views::transform([](auto&& v) { return std::stoull(v); })
            | to<std::vector<uint64_t>>;
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

    return 0;
}
