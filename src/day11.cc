#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/action/reverse.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>

#include <cstdint>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

using Coord = uint64_t;
using Point = Gfx_2d::Point<Coord>;
using Map = std::unordered_set<Point>;

Coord process(Map map, unsigned expand = 1)
{
    Coord max_x{0}, max_y{0};
    std::set<Coord> has_x, has_y;
    for (auto const& px : map) {
        has_x.insert(px.x);
        has_y.insert(px.y);

        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }

    std::vector<Coord> missing_x, missing_y;
    for (auto i : ranges::views::ints | ranges::views::take(max_x + 1)) {
        if (!has_x.contains(i)) {
            missing_x.push_back(i);
        }
    }
    for (auto i : ranges::views::ints | ranges::views::take(max_y + 1)) {
        if (!has_y.contains(i)) {
            missing_y.push_back(i);
        }
    }

    missing_x |= ranges::actions::reverse;
    missing_y |= ranges::actions::reverse;

    for (Coord xx : missing_x) {
        Map m2;
        for (auto px : map) {
            if (px.x > xx) {
                px.x += expand;
            }
            m2.insert(std::move(px));
        }
        std::swap(map, m2);
    }

    for (Coord yy : missing_y) {
        Map m2;
        for (auto px : map) {
            if (px.y > yy) {
                px.y += expand;
            }
            m2.insert(std::move(px));
        }
        std::swap(map, m2);
    }

    std::vector<Point> stars;
    for (auto px : map) {
        stars.push_back(std::move(px));
    }

    uint64_t sum{0};
    for (auto i1 = stars.begin(); i1 != stars.end(); ++i1) {
        for (auto i2 = std::next(i1); i2 != stars.end(); ++i2) {
            sum += i1->manhattan_dist(*i2);
        }
    }

    return sum;
}

int main()
{
    Map map;

    Coord y{0};

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        Coord x{0};
        for (auto c : line) {
            if (c == '#') {
                map.insert({x, y});
            }
            ++x;
        }
        ++y;
    }

    fmt::print("1: {}\n", process(map, 1));
    fmt::print(
        "2: {}\n",
        process(
            map, 1000000 - 1));  // -1 because part 2 require to replace a column with columns, process() is expanding

    return 0;
}
