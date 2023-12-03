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

    auto maybe_sum = [&sum](std::optional<unsigned> val) -> void {
        if (val) {
            sum += *val;
        }
    };

    auto parse_num = [&map](Point from) -> std::optional<unsigned> {
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
    };

    for (auto const& px : symbols) {
        auto val = parse_num({px + Gfx_2d::Left});
        maybe_sum(val);
        val = parse_num({px + Gfx_2d::Right});
        maybe_sum(val);
        val = parse_num({px + Gfx_2d::Up});
        if (!val) {
            val = parse_num({px + Gfx_2d::TopLeft});
            maybe_sum(val);
            val = parse_num({px + Gfx_2d::TopRight});
        }
        maybe_sum(val);
        val = parse_num({px + Gfx_2d::Down});
        if (!val) {
            val = parse_num({px + Gfx_2d::BottomLeft});
            maybe_sum(val);
            val = parse_num({px + Gfx_2d::BottomRight});
        }
        maybe_sum(val);
    }
    fmt::print("1: {}\n", sum);
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

    return 0;
}
