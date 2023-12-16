#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <unordered_set>
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
};

using Player = std::pair<Point, Gfx_2d::Direction>;

void part1(Map const& map)
{
    std::unordered_set<Point> energized;
    std::set<Player> cache;

    std::vector<Player> players;
    players.push_back({{}, Gfx_2d::Right});

    for(;!players.empty();) {
        std::vector<Player> next_round;

        for (Player const& p : players) {
            if (cache.insert(p).second == false) {
                continue;
            }

            auto const& me = p.first;

            if (me.x < 0 || me.x == map.end_x || me.y < 0 || me.y == map.end_y) {
                continue;
            }

            energized.insert(me);

            auto const& next = NEXT_MOVE.at(std::make_pair(map.at(me), p.second));
            auto dir = next.at(0);
            next_round.push_back({me+dir, dir});
            if (next.size() == 2) {
                auto d2 = next.at(1);
                next_round.push_back({me + d2, d2});
            }
        }
        std::swap(players, next_round);
    }
    fmt::print("Players {}, cache: {}\n", players.size(), cache.size());
    fmt::print("1: {}\n", energized.size());
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
