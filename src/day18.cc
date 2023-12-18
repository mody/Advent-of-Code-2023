#include "point2d.h"

#include <fmt/core.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/view/drop.hpp>

#include <cmath>
#include <iostream>
#include <map>
#include <vector>

using Coord = int64_t;
using Node = std::pair<Gfx_2d::Direction, Coord>;

static const std::map<unsigned char, Gfx_2d::Direction> TO_DIRECTION {
    {'R', Gfx_2d::Right}, {'D', Gfx_2d::Down}, {'L', Gfx_2d::Left}, {'U', Gfx_2d::Up},
    {'0', Gfx_2d::Right}, {'1', Gfx_2d::Down}, {'2', Gfx_2d::Left}, {'3', Gfx_2d::Up},
};

// thanks to https://topaz.github.io/paste/#XQAAAQCWAQAAAAAAAAA4GwhH7u5OdXNRlPeIKYn01V1xhpyceJIwtT5HvB9jA7zH7F67Rry3WbFjXXlg+rj3O3Pcr0jV1/vTXP/7jdZNkrMTQPkbSnuvlXFjzrTfvX3Ir+VxA2REBE39YUc3u64mdAog36DTi6Cd3mk6MAP8HSDqMVD0qJWDIkT/T1frVVjq6Vwl5B9bhr/EfMYhQ76SVYOJ6B7nLXSvqLGiHL5rPpGvcRdh0YoH1xiJatTkING88zdpuFObWwPvuD4cTrcfh/S8kGfa8HfqLQ2orimQG0QsuKI0HONFCDJETbWr//xp3YC3FDnfyWQt6ckdQnNJ6Oha4P/4frRL
int64_t process(std::vector<Node> const& plan)
{
    double pos {0}, result {1};
    for (auto const& node : plan) {
        pos += node.first.dx * node.second;
        result += node.first.dy * node.second * pos + node.second / 2.0;
    }
    return result;
}

int main()
{
    std::vector<Node> plan1, plan2;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);

        plan1.push_back({TO_DIRECTION.at(parts.at(0).at(0)), std::stoull(parts.at(1))});
        plan2.push_back(
            {TO_DIRECTION.at(parts.at(2).substr(7, 1).front()), std::stoull(parts.at(2).substr(2, 5), nullptr, 16)});
    }

    fmt::print("1: {}\n", process(plan1));
    fmt::print("2: {}\n", process(plan2));

    return 0;
}
