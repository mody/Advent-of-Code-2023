#include "point2d.h"

#include <fmt/core.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/view/drop.hpp>

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <unordered_set>
#include <vector>

using Coord = int64_t;
using Point = Gfx_2d::Point<Coord>;
using Points = std::vector<Point>;

int64_t shoelace(Points const& points)
{
    using namespace ranges;
    int64_t sum1{0}, sum2{0};

    Point p1 = points.front();
    for (Point p2 : points | views::drop(1)) {
        sum1 += p1.x * p2.y;
        sum2 += p1.y * p2.x;
        std::swap(p1, p2);
    }

    sum1 += p1.x * points.front().y;
    sum2 += p1.y * points.front().x;

    return std::abs(sum1 - sum2) / 2;
}

static const std::map<unsigned char, Gfx_2d::Direction> TO_DIRECTION {
    {'R', Gfx_2d::Right}, {'D', Gfx_2d::Down}, {'L', Gfx_2d::Left}, {'U', Gfx_2d::Up},
    {'0', Gfx_2d::Right}, {'1', Gfx_2d::Down}, {'2', Gfx_2d::Left}, {'3', Gfx_2d::Up},
};

int main()
{
    Points points;
    Point px{0,0};
    points.push_back(px);
    Coord total_length {0};

    Points points2;
    Point px2{0,0};
    points2.push_back(px2);
    Coord total_length2 {0};

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);

        auto dir {TO_DIRECTION.at(parts.at(0).at(0))};
        int64_t length = std::stoull(parts.at(1));

        dir *= length;
        px += dir;
        total_length += length;
        points.push_back(px);

        dir = TO_DIRECTION.at(parts.at(2).substr(7, 1).front());
        length = std::stoull(parts.at(2).substr(2, 5), nullptr, 16);
        dir *= length;
        px2 += dir;
        total_length2 += length;
        points2.push_back(px2);

    }

    // thanks to https://www.reddit.com/r/adventofcode/comments/18l0qtr/comment/kdveugr/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
    {
        auto area = shoelace(points);
        fmt::print("1: {}\n", area + total_length / 2 + 1);
    }

    {
        auto area2 = shoelace(points2);
        fmt::print("2: {}\n", area2 + total_length2 / 2 + 1);
    }


    return 0;
}
