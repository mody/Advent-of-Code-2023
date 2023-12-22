#include "point3d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/action/sort.hpp>
#include <range/v3/view/enumerate.hpp>

#include <cmath>
#include <iostream>
#include <set>
#include <vector>

using namespace ranges;

using Coord = int;
using Point = Gfx_3d::Point<Coord>;

using Cube = std::pair<Point, Point>;
using Cubes = std::vector<Cube>;

void part1(Cubes cubes)
{
    actions::sort(cubes, [](auto const& lhs, auto const& rhs) { return lhs.first.z < rhs.first.z; });

    Coord max_x {0}, max_y {0};
    for (auto const& cube : cubes) {
        max_x = std::max(max_x, cube.second.x);
        max_y = std::max(max_y, cube.second.y);
    }
    ++max_x;
    ++max_y;

    std::vector<std::vector<Coord>> height(max_y, std::vector<Coord>(max_x, 0));
    std::vector<std::vector<Coord>> ids(max_y, std::vector<Coord>(max_x, -1));
    std::set<Coord> singles;

    for (auto const& [cubeid, cube] : cubes | views::enumerate) {
        assert(cube.first.x <= cube.second.x);
        assert(cube.first.y <= cube.second.y);
        assert(cube.first.z <= cube.second.z);

        Coord collision_z {0};
        for (Coord y = cube.first.y; y <= cube.second.y; ++y) {
            for (Coord x = cube.first.x; x <= cube.second.x; ++x) {
                collision_z = std::max(collision_z, height.at(y).at(x));
            }
        }

        std::set<int> supported_by;
        for (Coord y = cube.first.y; y <= cube.second.y; ++y) {
            for (Coord x = cube.first.x; x <= cube.second.x; ++x) {
                if (height.at(y).at(x) == collision_z && ids.at(y).at(x) != -1) {
                    supported_by.insert(ids.at(y).at(x));
                }
                ids.at(y).at(x) = cubeid;
                height.at(y).at(x) = collision_z + 1 + (cube.second.z - cube.first.z);
            }
        }

        if (supported_by.size() == 1) {
            singles.insert(*supported_by.begin());
        }
    }

    fmt::print("1: {}\n", cubes.size() - singles.size());
}

int main()
{
    Cubes cubes;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(",~"), boost::token_compress_on);

        Point p1 {std::stoi(parts.at(0)), std::stoi(parts.at(1)), std::stoi(parts.at(2))},
            p2 {std::stoi(parts.at(3)), std::stoi(parts.at(4)), std::stoi(parts.at(5))};
        assert(p1.z <= p2.z);

        cubes.push_back({std::move(p1), std::move(p2)});
    }

    part1(cubes);

    return 0;
}
