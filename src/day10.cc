#include "point2d.h"

#include <fmt/core.h>

#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using Map = std::unordered_map<Point, unsigned char>;

#if 0
| is a vertical pipe connecting north and south.
- is a horizontal pipe connecting east and west.
L is a 90-degree bend connecting north and east.
J is a 90-degree bend connecting north and west.
7 is a 90-degree bend connecting south and west.
F is a 90-degree bend connecting south and east.
#endif

static const std::map<unsigned char, std::array<Gfx_2d::Direction, 2>> TO_MOVES
{
    {'|', {Gfx_2d::Up, Gfx_2d::Down}},
    {'-', {Gfx_2d::Left, Gfx_2d::Right}},
    {'L', {Gfx_2d::Up, Gfx_2d::Right}},
    {'J', {Gfx_2d::Up, Gfx_2d::Left}},
    {'7', {Gfx_2d::Down, Gfx_2d::Left}},
    {'F', {Gfx_2d::Down, Gfx_2d::Right}}
};


void part1(Map const& map, Point me)
{
    unsigned steps {0};
    Point start {me};

    for (Gfx_2d::Direction back {};; ++steps) {
        for (auto dir : TO_MOVES.at(map.at(me))) {
            if (dir == back) continue;
            Point next {me + dir};
            assert(next != me);
            back = dir.back();
            me = next;
            break;
        }
        if (me == start) {
            break;
        }
    }
    fmt::print("1: {}\n", steps / 2 + 1);
}

int main()
{
    Map map;
    Point me;

    {
        Coord y {0};
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) break;

            Coord x {0};
            for (auto c : line) {
                if (c == 'S') {
                    me = {x, y};
                }
                if (c != '.') {
                    map.insert({{x,y}, c});
                }

                ++x;
            }
            ++y;
        }
    }

    // Find what character starting tile has
    std::array<Gfx_2d::Direction, 2> possible_moves{};
    unsigned pos{0};
    for (auto dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right}) {
        Point next{me + dir};
        auto it = map.find(next);
        if (it == map.end()) continue;
        for (auto d2 : TO_MOVES.at(it->second)) {
            if (d2.back() == dir) {
                possible_moves.at(pos++) = dir;
                break;
            }
        }
    }
    for (auto const& [c, moves] : TO_MOVES) {
        if (moves == possible_moves) {
            map.at(me) = c;
            break;
        }
    }

    part1(map, me);

    return 0;
}
