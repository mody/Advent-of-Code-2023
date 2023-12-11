#include "point2d.h"

#include <fmt/core.h>

#include <iostream>
#include <unordered_set>
#include <vector>

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;
using Map = std::unordered_set<Point>;

void dump(Map const& map)
{
    Coord max_x{0}, max_y{0};
    for (auto const& px : map) {
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }

    for(Coord y = 0; y <= max_y; ++y) {
        for(Coord x = 0; x <= max_x; ++x) {
            fmt::print("{:c}", map.contains({x,y}) ? '#' : '.');
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

void part1(Map map)
{
    Coord max_x{0}, max_y{0};
    for (auto const& px : map) {
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }

    for(Coord y = 0; y <= max_y; ++y) {
        bool empty{true};
        for(Coord x = 0; x <= max_x; ++x) {
            if (map.contains({x, y})) {
                empty = false;
                break;
            }
        }
        if (empty) {
            Map m2;
            for (auto px : map) {
                if (px.y > y) {
                    ++px.y;
                }
                m2.insert(std::move(px));
            }
            ++y;
            ++max_y;
            std::swap(map, m2);
        }
    }

    for(Coord x = 0; x <= max_x; ++x) {
        bool empty{true};
        for(Coord y = 0; y <= max_y; ++y) {
            if (map.contains({x, y})) {
                empty = false;
                break;
            }
        }
        if (empty) {
            Map m2;
            for (auto px : map) {
                if (px.x > x) {
                    ++px.x;
                }
                m2.insert(std::move(px));
            }
            ++x;
            ++max_x;
            std::swap(map, m2);
        }
    }

    std::vector<Point> stars;
    for (auto px : map) {
        stars.push_back(std::move(px));
    }

    unsigned sum{0};
    for (auto i1 = stars.begin(); i1 != stars.end(); ++i1) {
        for (auto i2 = std::next(i1); i2 != stars.end(); ++i2) {
            sum += i1->manhattan_dist(*i2);
        }
    }

    fmt::print("1: {}\n", sum);
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

    part1(map);

    return 0;
}
