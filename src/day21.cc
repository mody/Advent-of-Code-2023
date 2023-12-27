#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/core.hpp>
#include <range/v3/view/map.hpp>

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

namespace {
template<class T1, class T2>
inline T1 mod(T1 a, T2 b)
{
    assert(b > 0);
    T1 ret = a % b;
    if constexpr (std::is_unsigned_v<T1>) {
        return ret;
    } else {
        return ret >= 0 ? ret : ret + b;
    }
}
}  // namespace

struct Map
{
    Coord end_x {0}, end_y {0};
    std::unordered_map<Point, unsigned char> map;

    void insert(Point px, unsigned char c) noexcept { map.insert({px, c}); }
    void erase(Point px) noexcept { map.erase(px); }

    bool contains(Point px) const noexcept { return map.contains(px); }

    unsigned char at(Point const& px) const { return map.at(px); }

    unsigned char infinity_at(Point px) const {
        px.x = mod(px.x, end_x);
        px.y = mod(px.y, end_y);

        return map.at(px);
    }

    void min_max() noexcept
    {
        for (auto const& [px, _] : map) {
            end_x = std::max(end_x, px.x);
            end_y = std::max(end_y, px.y);
        }
        ++end_x;
        ++end_y;
    }

    bool inside(Point const& px) const noexcept { return px.x >= 0 && px.x < end_x && px.y >= 0 && px.y < end_y; }

    void dump() const noexcept
    {
        for (Coord y = 0; y < end_y; ++y) {
            for (Coord x = 0; x < end_x; ++x) {
                const Point px {x, y};
                fmt::print("{:c}", map.at(px));
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    }

    void dump(std::unordered_set<Point> const& reached) const noexcept
    {
        for (Coord y = 0; y < end_y; ++y) {
            for (Coord x = 0; x < end_x; ++x) {
                const Point px {x, y};
                if (reached.contains(px)) {
                    fmt::print("O");
                } else {
                    fmt::print("{:c}", map.at(px));
                }
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    }
};

void part1_bfs(Map const& map, Point const& start)
{
    std::unordered_map<Point, unsigned> distances;
    distances.insert({start, 0});

    std::deque<Point> q;
    q.push_back(start);

    const unsigned MAX_STEPS {64};

    for (unsigned step {0}; step < MAX_STEPS; ++step) {
        std::deque<Point> q2;

        while (!q.empty()) {
            const Point me {q.front()};
            const unsigned distance {distances.at(me)};

            q.pop_front();

            for (auto dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right}) {
                const Point px {me + dir};
                const unsigned new_distance = distance + 1;
                const auto it {distances.find(px)};
                if (it == distances.end() || it->second > new_distance) {
                    if (map.at(px) == '.') {
                        distances.insert({px, 0}).first->second = new_distance;
                        q2.push_back(px);
                    }
                }
            }
        }
        std::swap(q, q2);
    }

    // map.dump(ranges::views::keys(distances) | ranges::to<std::unordered_set<Point>>);

    fmt::print("1: {}\n", ranges::count_if(distances, [](auto&& it) { return (it.second & 0x1) == 0; }));
}


void part2_bfs(Map const& map, Point const& start)
{
    std::unordered_map<Point, unsigned> distances;
    distances.insert({start, 0});

    std::deque<Point> q;
    q.push_back(start);

    const unsigned MIN_STEPS {131};

    for (unsigned step {0}; step < MIN_STEPS; ++step) {
        std::deque<Point> q2;

        while (!q.empty()) {
            const Point me {q.front()};
            const unsigned distance {distances.at(me)};

            q.pop_front();

            for (auto dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right}) {
                const Point px {me + dir};
                const unsigned new_distance = distance + 1;
                const auto it {distances.find(px)};
                if (it == distances.end() || it->second > new_distance) {
                    if (map.infinity_at(px) == '.') {
                        distances.insert({px, 0}).first->second = new_distance;
                        q2.push_back(px);
                    }
                }
            }
        }
        std::swap(q, q2);
    }

    uint64_t odd_full {0}, even_full {0}, odd_corners {0}, even_corners {0};
    ranges::for_each(distances, [&map, &odd_full, &even_full, &odd_corners, &even_corners](auto&& it) {
        if (map.inside(it.first)) {
            if ((it.second & 0x1) == 0) {
                // even
                ++even_full;
                if (it.second > 65) ++even_corners;
            } else {
                // odd
                ++odd_full;
                if (it.second > 65) ++odd_corners;
            }
        }
    });

    // fmt::print(
    //     "odd_full {}, even_full {}, odd_corners {}, even_corners {}\n", odd_full, even_full, odd_corners, even_corners);

    const uint64_t MAX_STEPS {26501365};
    const uint64_t TILES {MAX_STEPS / 131};
    assert(TILES == 202300);

    // Thanks to https://github.com/villuna/aoc23/wiki/A-Geometric-solution-to-advent-of-code-2023,-day-21
    const uint64_t result2 {
        (((TILES + 1) * (TILES + 1)) * odd_full) + ((TILES * TILES) * even_full) - ((TILES + 1) * odd_corners)
        + (TILES * even_corners)};

    fmt::print("2: {}\n", result2 - TILES);
    // 610158187362102 !!
}

int main()
{
    Map map;
    Point start;

    {
        Coord y {0};
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) break;
            Coord x{0};
            for (auto c : line) {
                if (c == 'S') {
                    start = {x,y};
                    map.insert({x,y}, '.');
                } else {
                    map.insert({x,y}, c);
                }
                ++x;
            }
            ++y;
        }
    }
    map.min_max();

    part1_bfs(map, start);
    part2_bfs(map, start);

    return 0;
}
