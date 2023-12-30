#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <functional>
#include <iostream>
#include <map>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/all.hpp>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using NodeId = unsigned;

#if 0

Node Start
|
V
Node crossroads1

#endif

struct Destination {
    NodeId node_id{0};
    unsigned distance{0};
};

struct Node
{
    std::vector<Destination> children;
};
using Tree = std::map<NodeId, Node>;

struct Map {
    std::vector<std::string> data;
    Coord end_x{0}, end_y{0};
    Tree tree;
    Point finish;

    std::vector<unsigned> paths;

    void min_max() noexcept
    {
        end_x = data.front().size();
        end_y = data.size();

        finish = {end_x - 2, end_y - 1};
    }

    void dump() const noexcept
    {
        for (auto const& line : data) {
            fmt::print("{}\n", line);
        }
    }

    unsigned char at(Point const& px) const { return data.at(px.y).at(px.x); }

    bool contains(Point const& px) const noexcept { return px.x >= 0 && px.x < end_x && px.y >= 0 && px.y < end_y; }

    void intersection_tree() noexcept { inspect_path({1, 0}, Gfx_2d::Down, 0); }

private:
    void inspect_path(Point from, Gfx_2d::Direction direction, unsigned distance) noexcept
    {
        if (from == finish) {
            // handle destination
            // fmt::print("FOUND END at ({},{}) distance {}\n", from.x, from.y, distance);
            paths.push_back(distance);
            return;
        }

        std::vector<Gfx_2d::Direction> moves;
        for (;;) {
            moves.clear();

            if (from == finish) {
                // handle destination
                // fmt::print("FOUND END at ({},{}) distance {}\n", from.x, from.y, distance);
                paths.push_back(distance);
                return;
            }

            assert(at(from) != '#');

            static const std::map<unsigned char, Gfx_2d::Direction> TO_DIR{
                {'^', Gfx_2d::Up},
                {'v', Gfx_2d::Down},
                {'<', Gfx_2d::Left},
                {'>', Gfx_2d::Right},

            };

            auto it = TO_DIR.find(at(from));
            bool slope {it != TO_DIR.end()};
            if (slope) {
                direction = it->second;
            }

            if (Point px {from + direction}; at(px) != '#') {
                moves.push_back(direction);
            }

            if (!slope) {
                if (Point px {from + direction.cw90()}; at(px) != '#') {
                    it = TO_DIR.find(at(px));
                    if (it == TO_DIR.end() || (px + it->second) != from) {
                        moves.push_back(direction.cw90());
                    }
                }
                if (Point px {from + direction.ccw90()}; at(px) != '#') {
                    it = TO_DIR.find(at(px));
                    if (it == TO_DIR.end() || (px + it->second) != from) {
                        moves.push_back(direction.ccw90());
                    }
                }
            }

            // fmt::print("At ({},{}), distance {}, moves {}\n", from.x, from.y, distance, moves.size());

            if (moves.size() > 1) {
                break;
            } else {
                ++distance;
                direction = moves.front();
                from += direction;
            }
        }
        // fmt::print("Found crossroads at ({},{}), distance {}\n", from.x, from.y, distance);

        ++distance;
        for (auto d : moves) {
            inspect_path(from + d, d, distance);
        }
    }
};

void part1(Map map)
{
    map.intersection_tree();

    ranges::sort(map.paths, std::greater{});

    // fmt::print("paths: {}\n", ranges::views::all(map.paths));
    fmt::print("1: {}\n", map.paths.front());
}

int main()
{
    Map map;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;
        map.data.push_back(std::move(line));
    }
    map.min_max();

    part1(map);

    return 0;
}
