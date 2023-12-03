#include "point2d.h"

#include <fmt/core.h>

#include <cctype>
#include <deque>
#include <iostream>
#include <optional>
#include <unordered_map>

using Point = Gfx_2d::Point<unsigned>;
using Map = std::unordered_map<Point, char>;

namespace {

constexpr unsigned to_num(char c) noexcept
{
    return c - '0';
}

std::optional<unsigned> parse_num(Map const& map, Point from) noexcept
{
    auto it {map.find(from)};
    if (it == map.end() || !std::isdigit(it->second)) {
        return std::nullopt;
    }
    while (map.contains(from) && std::isdigit(map.at(from))) {
        from += Gfx_2d::Left;
    }
    Point to {from + Gfx_2d::Right};
    while (map.contains(to) && std::isdigit(map.at(to))) {
        to += Gfx_2d::Right;
    }
    to += Gfx_2d::Left;
    unsigned val {0};
    unsigned mul {1};
    while (to != from) {
        val += mul * to_num(map.at(to));
        mul *= 10;
        to += Gfx_2d::Left;
    }
    return val;
}

void for_each_num(Map const& map, Point px, auto&& fn) noexcept(noexcept(fn(0)))
{
    auto val = parse_num(map, {px + Gfx_2d::Left});
    if (val) fn(*val);
    val = parse_num(map, {px + Gfx_2d::Right});
    if (val) fn(*val);
    val = parse_num(map, {px + Gfx_2d::Up});
    if (!val) {
        val = parse_num(map, {px + Gfx_2d::TopLeft});
        if (val) fn(*val);
        val = parse_num(map, {px + Gfx_2d::TopRight});
    }
    if (val) fn(*val);
    val = parse_num(map, {px + Gfx_2d::Down});
    if (!val) {
        val = parse_num(map, {px + Gfx_2d::BottomLeft});
        if (val) fn(*val);
        val = parse_num(map, {px + Gfx_2d::BottomRight});
    }
    if (val) fn(*val);
}

}  // namespace

void part1(Map const& map)
{
    std::deque<Point> symbols;
    for (auto const& [px, c] : map) {
        if (std::isdigit(c)) {
            continue;
        }
        symbols.push_back(px);
    }

    unsigned sum {0};

    for (auto const& px : symbols) {
        for_each_num(map, px, [&sum](unsigned val) { sum += val; });
    }
    fmt::print("1: {}\n", sum);
}

void part2(Map const& map)
{
    std::deque<Point> symbols;
    for (auto const& [px, c] : map) {
        if (c == '*') symbols.push_back(px);
    }

    uint64_t sum {0};

    for (auto const& px : symbols) {
        std::deque<uint64_t> values;
        for_each_num(map, px, [&values](unsigned val) { values.push_back(val); });

        if (values.size() == 2) {
            sum += values.at(0) * values.at(1);
        }
    }

    fmt::print("2: {}\n", sum);
}

int main()
{
    Map map;

    unsigned y {0};
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        unsigned x {0};
        for (auto const& c : line) {
            if (c != '.') {
                map.insert({{x, y}, c});
            }
            ++x;
        }
        ++y;
    }

    part1(map);
    part2(map);

    return 0;
}
