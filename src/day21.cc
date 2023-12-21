#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <unordered_set>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

struct Map
{
    Coord end_x {0}, end_y {0};
    std::unordered_set<Point> map;

    void insert(Point px) noexcept { map.insert(px); }
    void erase(Point px) noexcept { map.erase(px); }

    bool contains(Point px) const noexcept { return map.contains(px); }

    void min_max() noexcept
    {
        for (auto const& px : map) {
            end_x = std::max(end_x, px.x);
            end_y = std::max(end_y, px.y);
        }
        ++end_x;
        ++end_y;
    }

    void dump() const noexcept
    {
        for (Coord y = 0; y < end_y; ++y) {
            for (Coord x = 0; x < end_x; ++x) {
                const Point px {x, y};
                const auto it = map.find(px);
                fmt::print("{}", (it == map.end()) ? '.' : '#');
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
                    const auto it = map.find(px);
                    fmt::print("{}", (it == map.end()) ? '.' : '#');
                }
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    }
};

void part1(Map const& map, Point const& start)
{
    std::unordered_set<Point> reached;
    reached.insert(start);

    for (unsigned i = 0; i < 64; ++i) {
        std::unordered_set<Point> r2;
        for (auto const& me : reached) {
            for (auto dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right}) {
                Point px {me + dir};
                if (!map.contains(px)) {
                    r2.insert(std::move(px));
                }
            }
        }
        std::swap(reached, r2);
    }

    fmt::print("1: {}\n", reached.size());
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
                if (c == '#') {
                    map.insert({x,y});
                } else if (c == 'S') {
                    start = {x,y};
                }
                ++x;
            }
            ++y;
        }
    }
    map.min_max();

    part1(map, start);

    return 0;
}
