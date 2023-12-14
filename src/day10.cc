#include "point2d.h"

#include <fmt/core.h>

#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using Map = std::unordered_map<Point, unsigned char>;


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


void part2(Map map, Point me)
{
    std::unordered_set<Point> path;
    Point start {me};

    path.insert(start);

    unsigned char last_corner {0};
    Gfx_2d::Direction last_move{};

    for (Gfx_2d::Direction back {};;) {
        for (auto dir : TO_MOVES.at(map.at(me))) {
            if (dir == back) continue;
            Point next {me + dir};
            assert(next != me);
            back = dir.back();

            unsigned char& c = map.at(me);

            if (c == '|') {
                c = '!';
                last_corner = ' ';
            } else if (c == '7' && last_corner == 'L' && last_move == Gfx_2d::Right) {
                c = '!';
                last_corner = ' ';
            } else if (c == 'L' && last_corner == '7' && last_move == Gfx_2d::Left) {
                c = '!';
                last_corner = ' ';
            } else if (c == 'J' && last_corner == 'F' && last_move == Gfx_2d::Right) {
                c = '!';
                last_corner = ' ';
            } else if (c == 'F' && last_corner == 'J' && last_move == Gfx_2d::Left) {
                c = '!';
                last_corner = ' ';
            }

            if (c == 'L' || c == 'F' || c == '7' || c == 'J') last_corner = c;

            last_move = dir;
            path.insert(me);
            me = next;
            break;
        }
        if (me == start) {
            break;
        }
    }

    Coord max_x{0}, max_y{0};
    for (auto const& [px, _] : map) {
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }

    // remove non-path piping
    for(Coord y = 0; y <= max_y; ++y) {
        for(Coord x = 0; x <= max_x; ++x) {
            const Point px {x, y};
            if (map.contains(px) && !path.contains(px)) {
                map.erase(px);
            }
        }
    }

    unsigned count{0};
    for(Coord y = 0; y <= max_y; ++y) {
        bool inside{false};
        for(Coord x = 0; x <= max_x; ++x) {
            const Point px {x, y};
            auto it = map.find(px);
            if (it == map.end()) {
                if (inside) {
                    map.insert({px, 'I'});
                    ++count;
                }
            } else if (it->second == '!') {
                inside = !inside;
            }
        }
    }

    fmt::print("2: {}\n", count);
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
                    map.insert({{x,y}, static_cast<unsigned char>(c)});
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
    part2(map, me);

    return 0;
}
