#include "point2d.h"

#include <fmt/core.h>

#include <range/v3/action/sort.hpp>

#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/parallel_for_each.h>

#include <iostream>
#include <map>
#include <memory>
#include <range/v3/functional/comparisons.hpp>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

static const std::map<std::pair<unsigned char, Gfx_2d::Direction>, std::vector<Gfx_2d::Direction>> NEXT_MOVE {
    {std::make_pair('\\', Gfx_2d::Right), {Gfx_2d::Down}},
    {std::make_pair('\\', Gfx_2d::Up), {Gfx_2d::Left}},
    {std::make_pair('\\', Gfx_2d::Down), {Gfx_2d::Right}},
    {std::make_pair('\\', Gfx_2d::Left), {Gfx_2d::Up}},
    {std::make_pair('/', Gfx_2d::Up), {Gfx_2d::Right}},
    {std::make_pair('/', Gfx_2d::Left), {Gfx_2d::Down}},
    {std::make_pair('/', Gfx_2d::Right), {Gfx_2d::Up}},
    {std::make_pair('/', Gfx_2d::Down), {Gfx_2d::Left}},
    {std::make_pair('-', Gfx_2d::Left), {Gfx_2d::Left}},
    {std::make_pair('-', Gfx_2d::Right), {Gfx_2d::Right}},
    {std::make_pair('|', Gfx_2d::Up), {Gfx_2d::Up}},
    {std::make_pair('|', Gfx_2d::Down), {Gfx_2d::Down}},

    {std::make_pair('-', Gfx_2d::Up), {Gfx_2d::Left, Gfx_2d::Right}},
    {std::make_pair('-', Gfx_2d::Down), {Gfx_2d::Left, Gfx_2d::Right}},

    {std::make_pair('|', Gfx_2d::Left), {Gfx_2d::Up, Gfx_2d::Down}},
    {std::make_pair('|', Gfx_2d::Right), {Gfx_2d::Up, Gfx_2d::Down}},

    {std::make_pair('.', Gfx_2d::Left), {Gfx_2d::Left}},
    {std::make_pair('.', Gfx_2d::Right), {Gfx_2d::Right}},
    {std::make_pair('.', Gfx_2d::Up), {Gfx_2d::Up}},
    {std::make_pair('.', Gfx_2d::Down), {Gfx_2d::Down}},
};

struct Map
{
    std::vector<std::string> data;
    int end_x {0}, end_y {0};

    void min_max() noexcept
    {
        end_x = data.front().size();
        end_y = data.size();
    }

    unsigned char at(Point const& px) const { return data.at(px.y).at(px.x); }

    unsigned raytrace(Point px, Gfx_2d::Direction where) const;
};

using Player = std::pair<Point, Gfx_2d::Direction>;

unsigned Map::raytrace(Point px, Gfx_2d::Direction where) const
{
    std::unordered_set<Point> energized;
    std::set<Player> cache;

    std::vector<Player> players;
    players.push_back({px, where});

    for (; !players.empty();) {
        std::vector<Player> next_round;

        for (Player const& p : players) {
            if (cache.insert(p).second == false) {
                continue;
            }

            auto const& me = p.first;

            if (me.x < 0 || me.x == end_x || me.y < 0 || me.y == end_y) {
                continue;
            }

            energized.insert(me);

            auto const& next_moves = NEXT_MOVE.at(std::make_pair(at(me), p.second));
            auto dir = next_moves.at(0);
            next_round.push_back({me + dir, dir});
            if (next_moves.size() == 2) {
                auto d2 = next_moves.at(1);
                next_round.push_back({me + d2, d2});
            }
        }
        std::swap(players, next_round);
    }

    return energized.size();
}

void part1(Map const& map)
{
    fmt::print("1: {}\n", map.raytrace({}, Gfx_2d::Right));
}

void part2(Map const& map)
{
    std::vector<Player> starting_positions;

    for (Coord x = 0; x < map.end_x; ++x) {
        starting_positions.push_back(std::make_pair(Point {x, 0}, Gfx_2d::Down));
        starting_positions.push_back(std::make_pair(Point {x, map.end_y - 1}, Gfx_2d::Up));
    }

    for (Coord y = 0; y < map.end_y; ++y) {
        starting_positions.push_back(std::make_pair(Point {0, y}, Gfx_2d::Right));
        starting_positions.push_back(std::make_pair(Point {map.end_x - 1, y}, Gfx_2d::Left));
    }

    std::vector<unsigned> results;
    for (auto const& p : starting_positions) {
        results.push_back(map.raytrace(p.first, p.second));
    }

    ranges::sort(results, ranges::greater{});
    fmt::print("2: {}\n", results.front());
}

void part2_tbb(Map const& map)
{
    std::vector<Player> starting_positions;

    for (Coord x = 0; x < map.end_x; ++x) {
        starting_positions.push_back(std::make_pair(Point {x, 0}, Gfx_2d::Down));
        starting_positions.push_back(std::make_pair(Point {x, map.end_y - 1}, Gfx_2d::Up));
    }

    for (Coord y = 0; y < map.end_y; ++y) {
        starting_positions.push_back(std::make_pair(Point {0, y}, Gfx_2d::Right));
        starting_positions.push_back(std::make_pair(Point {map.end_x - 1, y}, Gfx_2d::Left));
    }

    oneapi::tbb::concurrent_vector<unsigned> results;
    oneapi::tbb::parallel_for_each(
        starting_positions.begin(), starting_positions.end(),
        [&results, &map](Player const& p) { results.push_back(map.raytrace(p.first, p.second)); });

    ranges::sort(results, ranges::greater{});
    fmt::print("2: {}\n", results.front());
}

int main()
{
    Map map;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        map.data.push_back(std::move(line));
    }
    map.min_max();

    part1(map);
    part2_tbb(map);

    return 0;
}
