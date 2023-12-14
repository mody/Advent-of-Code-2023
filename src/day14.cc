#include "point2d.h"

#include <fmt/core.h>

#include <range/v3/view/enumerate.hpp>

#include <iostream>
#include <unordered_map>
#include <vector>

using namespace ranges;

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;

using Map = std::unordered_map<Point, char>;

void dump(Map const& map)
{
    Coord max_x{0}, max_y{0};
    for (auto const& [px, _] : map) {
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }

    for (Coord y = 0; y <= max_y; ++y) {
        for (Coord x = 0; x <= max_x; ++x) {
            const Point px {x, y};
            const auto it = map.find(px);
            fmt::print("{}", (it == map.end()) ? '.' : it->second);
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

void part1(Map map)
{
    Coord max_x{0}, max_y{0};
    for (auto const& [px, _] : map) {
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }
    ++max_x;
    ++max_y;

    for(Coord y = 1; y < max_y; ++y) {
        for(Coord x = 0; x < max_x; ++x) {
            Point px {x, y};
            auto it = map.find(px);
            if (it == map.end() || it->second != 'O') continue;
            map.erase(px);
            for (;;) {
                Point p2 = px + Gfx_2d::North;
                if (map.contains(p2)) break;
                px = p2;
                if (px.y == 0) break;
            }
            map.insert({px, 'O'});
        }
    }

    unsigned weight {0u};

    for (auto const& [px, c] : map) {
        if (c == 'O') {
            weight += max_y - px.y;
        }
    }
    fmt::print("1: {}\n", weight);
}

int main()
{
    Map map;

    {
        Coord y {0};
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) break;

            for (auto [x, c] : views::enumerate(line)) {
                if (c != '.') map.insert({{static_cast<Coord>(x), y}, c});
            }
            ++y;
        }
    }

    part1(map);

    return 0;
}
