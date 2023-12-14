#include "point2d.h"

#include <fmt/core.h>

#include <range/v3/view/enumerate.hpp>

#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

using namespace ranges;

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

struct Map
{
    Coord end_x {0}, end_y {0};
    std::unordered_map<Point, char> map;

    void insert(Point px, char c) noexcept { map.insert({px, c}); }
    void erase(Point px) noexcept { map.erase(px); }

    void min_max() noexcept
    {
        for (auto const& [px, _] : map) {
            end_x = std::max(end_x, px.x);
            end_y = std::max(end_y, px.y);
        }
        ++end_x;
        ++end_y;
    }

    void roll_north() noexcept
    {
        for (Coord y = 1; y < end_y; ++y) {
            for (Coord x = 0; x < end_x; ++x) {
                Point px {x, y};
                auto it = map.find(px);
                if (it == map.end() || it->second != 'O') continue;
                erase(px);
                for (;;) {
                    Point p2 = px + Gfx_2d::North;
                    if (map.contains(p2)) break;
                    px = p2;
                    if (px.y == 0) break;
                }
                insert(px, 'O');
            }
        }
    }

    void roll_west() noexcept
    {
        for (Coord x = 1; x < end_x; ++x) {
            for (Coord y = 0; y < end_y; ++y) {
                Point px {x, y};
                auto it = map.find(px);
                if (it == map.end() || it->second != 'O') continue;
                erase(px);
                for (;;) {
                    Point p2 = px + Gfx_2d::West;
                    if (map.contains(p2)) break;
                    px = p2;
                    if (px.x == 0) break;
                }
                insert(px, 'O');
            }
        }
    }

    void roll_south() noexcept
    {
        for (Coord y = end_y - 2; y >= 0; --y) {
            for (Coord x = 0; x < end_x; ++x) {
                Point px {x, y};
                auto it = map.find(px);
                if (it == map.end() || it->second != 'O') continue;
                erase(px);
                for (;;) {
                    Point p2 = px + Gfx_2d::South;
                    if (map.contains(p2)) break;
                    px = p2;
                    if (px.y == end_y - 1) break;
                }
                insert(px, 'O');
            }
        }
    }

    void roll_east() noexcept
    {
        for (Coord x = end_x - 2; x >= 0; --x) {
            for (Coord y = 0; y < end_y; ++y) {
                Point px {x, y};
                auto it = map.find(px);
                if (it == map.end() || it->second != 'O') continue;
                erase(px);
                for (;;) {
                    Point p2 = px + Gfx_2d::East;
                    if (map.contains(p2)) break;
                    px = p2;
                    if (px.x == end_x - 1) break;
                }
                insert(px, 'O');
            }
        }
    }

    void cycle() noexcept
    {
        roll_north();
        roll_west();
        roll_south();
        roll_east();
    }

    void dump() const noexcept
    {
        for (Coord y = 0; y < end_y; ++y) {
            for (Coord x = 0; x < end_x; ++x) {
                const Point px {x, y};
                const auto it = map.find(px);
                fmt::print("{}", (it == map.end()) ? '.' : it->second);
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    }

    unsigned count_weight() const noexcept
    {
        unsigned weight {0u};

        for (auto const& [px, c] : map) {
            if (c == 'O') {
                weight += end_y - px.y;
            }
        }
        return weight;
    }

    size_t hash() const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, end_x);
        boost::hash_combine(seed, end_y);
        for (auto const& [px, _] : map) {
            boost::hash_combine(seed, std::hash<Point>{}(px));
        }
        return seed;
    }
};


void part1(Map map)
{
    map.roll_north();
    fmt::print("1: {}\n", map.count_weight());
}

void part2(Map map)
{
    std::set<size_t> visited;

    uint64_t CYCLES {1000000000};

    uint64_t i {1};
    for (; i <= CYCLES; ++i) {
        map.cycle();
        if (!visited.insert(map.hash()).second) {
            break;
        }
    }

    fmt::print("Found 1st cycle at {}\n", i);

    visited.clear();
    visited.insert(map.hash());

    CYCLES -= i;
    uint64_t j {1};
    for (; j <= CYCLES; ++j) {
        map.cycle();
        if (!visited.insert(map.hash()).second) {
            break;
        }
    }
    fmt::print("Found 2nd cycle at {}\n", j);

    uint64_t k{0};
    CYCLES = CYCLES % j;

    for (; k < CYCLES; ++k) {
        map.cycle();
    }

    fmt::print("2: {}\n", map.count_weight());
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
                if (c != '.') map.insert({static_cast<Coord>(x), y}, c);
            }
            ++y;
        }
    }

    map.min_max();

    part1(map);
    part2(map);

    return 0;
}
