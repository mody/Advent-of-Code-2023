#include "point2d.h"

#include <boost/container_hash/hash.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/all.hpp>

#include <deque>
#include <iostream>
#include <map>
#include <unordered_set>
#include <utility>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

struct Map {
    void push_back(std::string&& line) noexcept { data.push_back(std::move(line)); }

    void min_max() noexcept
    {
        end_x = data.front().size();
        end_y = data.size();

        finish = {end_x - 2, end_y - 1};
    }

    unsigned char at(Point const& px) const { return data.at(px.y).at(px.x); }

    bool contains(Point const& px) const noexcept { return px.x >= 0 && px.x < end_x && px.y >= 0 && px.y < end_y; }

    void part1() noexcept
    {
        paths.clear();

        inspect_path1({1, 0}, Gfx_2d::Down, 0);

        fmt::print("1: {}\n", *ranges::max_element(paths));
    }

    void part2() noexcept
    {
        build_tree();

        paths.clear();

        std::unordered_set<Point, boost::hash<Point>> visited;
        visited.insert({1,0});
        inspect_path2({1, 0}, 0, visited);
        fmt::print("2: {}\n", *ranges::max_element(paths));
    }

private:
    void inspect_path1(Point from, Gfx_2d::Direction direction, unsigned distance) noexcept
    {
        if (from == finish) {
            // handle destination
            paths.push_back(distance);
            return;
        }

        std::vector<Gfx_2d::Direction> moves;
        for (;;) {
            moves.clear();

            if (from == finish) {
                // handle destination
                paths.push_back(distance);
                return;
            }

            assert(at(from) != '#');

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

            if (moves.size() > 1) {
                break;
            } else {
                ++distance;
                direction = moves.front();
                from += direction;
            }
        }

        ++distance;
        for (auto d : moves) {
            inspect_path1(from + d, d, distance);
        }
    }

    void inspect_path2(Point me, unsigned distance, std::unordered_set<Point, boost::hash<Point>>& visited) noexcept
    {
        if (me == finish) {
            paths.push_back(distance);
            return;
        }

        for (auto const& [to, dist] : tree.at(me)) {
            if (visited.insert(to).second) {
                inspect_path2(to, distance + dist, visited);
                visited.erase(to);
            }
        }
    }

    void build_tree() noexcept
    {
        tree.clear();

        struct QItem
        {
            Point at {};
            unsigned distance {0};
            Gfx_2d::Direction dir {};
        };

        std::deque<QItem> q;
        q.push_back({{1, 0}, 0, Gfx_2d::Down});  // starting point

        std::unordered_set<Point> visited;

        while (!q.empty()) {
            auto [me, distance, direction] = q.front();
            q.pop_front();

            const auto from{me};
            const auto traveled{distance};

            me += direction;
            ++distance;

            std::vector<Gfx_2d::Direction> moves;
            for (;;) {
                moves.clear();

                if (me == finish) {
                    // handle destination
                    break;
                }

                assert(at(me) != '#');

                if (Point px {me + direction}; at(px) != '#') {
                    moves.push_back(direction);
                }
                if (Point px {me + direction.cw90()}; at(px) != '#') {
                    moves.push_back(direction.cw90());
                }
                if (Point px {me + direction.ccw90()}; at(px) != '#') {
                    moves.push_back(direction.ccw90());
                }

                if (moves.size() > 1) {
                    break;
                } else {
                    ++distance;
                    direction = moves.front();
                    me += direction;
                }
            }

            tree[from][me] = distance - traveled;
            tree[me][from] = distance - traveled;

            if (me == finish) {
                break;
            } else {
                assert(moves.size() > 1);

                if (visited.insert(me).second) {
                    for (auto d : moves) {
                        q.push_back({me, distance, d});
                    }
                }
            }
        }
    }

private:
    std::vector<std::string> data;
    Coord end_x{0}, end_y{0};
    Point finish;

    std::vector<unsigned> paths;

    std::unordered_map<Point, std::unordered_map<Point, unsigned>> tree;

    static const std::map<unsigned char, Gfx_2d::Direction> TO_DIR;
};

const std::map<unsigned char, Gfx_2d::Direction> Map::TO_DIR {
    {'^', Gfx_2d::Up},
    {'v', Gfx_2d::Down},
    {'<', Gfx_2d::Left},
    {'>', Gfx_2d::Right},
};


int main()
{
    Map map;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;
        map.push_back(std::move(line));
    }
    map.min_max();

    map.part1();
    map.part2();

    return 0;
}
