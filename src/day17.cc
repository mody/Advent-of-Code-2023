#include "point2d.h"

#include <fmt/core.h>

#include <map>
#include <range/v3/action/sort.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/take_last.hpp>

#include <deque>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

struct Map
{
    std::vector<std::string> data;
    Coord end_x {0}, end_y {0};

    void min_max() noexcept
    {
        end_x = data.front().size();
        end_y = data.size();
    }

    unsigned char at(Point const& px) const { return data.at(px.y).at(px.x); }

    bool is_valid(Point const& px) const noexcept { return px.x >= 0 && px.y >= 0 && px.x < end_x && px.y < end_y; }
};

struct State
{
    Point px {};
    Gfx_2d::Direction dir;
    unsigned dist {0};

    constexpr auto operator<=>(State const&) const = default;

    void move() noexcept { px += dir; };
};

namespace std {
template<>
struct hash<State>
{
    size_t operator()(State const& s) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, s.px);
        boost::hash_combine(seed, s.dir);
        boost::hash_combine(seed, s.dist);
        return seed;
    }
};
}  // namespace std

// Thanks to https://www.reddit.com/r/adventofcode/comments/18luw6q/2023_day_17_a_longform_tutorial_on_day_17/
void part1(Map const& map)
{
    std::multimap<unsigned, State> queue;
    std::unordered_map<State, unsigned> seen;

    const Point finish {map.end_x - 1, map.end_y - 1};
    State end_state;

    auto add_state = [&queue, &seen, &map](unsigned cost, State s) {
        s.move();
        if (!map.is_valid(s.px)) return;

        cost += map.at(s.px) - '0';

        if (seen.insert({s, cost}).second) {
            queue.insert({cost, std::move(s)});
        }
    };

    add_state(0, {{0,0}, Gfx_2d::Down, 1});
    add_state(0, {{0,0}, Gfx_2d::Right, 1});

    for(bool done{false}; !done && !queue.empty();) {
        auto it = queue.begin();
        const unsigned cost{it->first};

        for(; it != queue.end() && it->first == cost; ++it) {
            State const& state {it->second};

            if (state.px == finish) {
                end_state = state;
                done = true;
                it = queue.begin();
                break;
            }

            add_state(cost, {state.px, state.dir.cw90(), 1});
            add_state(cost, {state.px, state.dir.ccw90(), 1});

            if (state.dist < 3) {
                add_state(cost, {state.px, state.dir, state.dist + 1});
            }
        }
        queue.erase(queue.begin(), it);
    }

    fmt::print("1: {}\n", seen.at(end_state));
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

    return 0;
}
