#include "point2d.h"
#include "point3d.h"

#include <cmath>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/transform.hpp>

#include <iostream>
#include <utility>
#include <vector>

using Coord = int64_t;
using Point2 = Gfx_2d::Point<Coord>;
using Point3 = Gfx_3d::Point<Coord>;

using Storm2 = std::pair<Point2, Gfx_2d::Direction>;
using Storm3 = std::pair<Point3, Gfx_3d::Direction>;

using Map2d = std::vector<Storm2>;
using Map3d = std::vector<Storm3>;


void part1(Map2d map2)
{
    // constexpr Coord MIN_X{7}, MAX_X{27};
    // constexpr Coord MIN_Y{7}, MAX_Y{27};
    constexpr Coord MIN_X{200000000000000}, MAX_X{400000000000000};
    constexpr Coord MIN_Y{200000000000000}, MAX_Y{400000000000000};

    unsigned cnt{0};

    for (auto i1 = map2.begin(); i1 != map2.end(); ++i1) {
        auto const& [p1, d1] = *i1;
        auto const p2 = p1 + d1;
        auto const& [x1, y1] = p1;
        auto const& [x2, y2] = p2;

        for (auto i2 = std::next(i1); i2 != map2.end(); ++i2) {
            auto const& [p3, d3] = *i2;
            auto const p4 = p3 + d3;
            auto const& [x3, y3] = p3;
            auto const& [x4, y4] = p4;

            // fmt::print("({},{}) vs ({},{})\n", x1, y1, x3, y3);

            const Coord delitel = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (delitel == 0) {
                // fmt::print("NO INTERSECTION on X\n");
                continue;
            }

            const Coord xh = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
            const Coord yh = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);

            const double ix = xh / static_cast<double>(delitel);
            const double iy = yh / static_cast<double>(delitel);
            // fmt::print("INTERSECTION ({:.3f},{:.3f})\n", ix, iy);

            // fmt::print(
            //     "p1 sign: {}, {}\n",
            //     std::signbit(d1.dx) == std::signbit(ix - x1),
            //     std::signbit(d1.dy) == std::signbit(iy - y1));
            // fmt::print(
            //     "p2 sign: {}, {}\n",
            //     std::signbit(d3.dx) == std::signbit(ix - x3),
            //     std::signbit(d3.dy) == std::signbit(iy - y3));

            // fmt::print("p1 diff {:.3f},{:.3f}\n", x1 - ix, y1 - iy);
            // fmt::print("p2 diff {:.3f},{:.3f}\n", x3 - ix, y3 - iy);

            if (
                std::signbit(d1.dx) == std::signbit(ix - x1) &&
                std::signbit(d1.dy) == std::signbit(iy - y1) &&
                std::signbit(d3.dx) == std::signbit(ix - x3) &&
                std::signbit(d3.dy) == std::signbit(iy - y3) &&
                ix >= MIN_X && ix <= MAX_X &&
                iy >= MIN_Y && iy <= MAX_Y)
            {
                ++cnt;
            }
        }
    }

    // 19978 TOO LOW
    // 28266!!
    fmt::print("1: {}\n", cnt);
}

int main()
{
    Map2d map2;
    Map3d map3;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" ,@"), boost::token_compress_on);

        auto&& data = parts
            | ranges::views::transform(([](auto&& val) { return std::stoll(val); }))
            | ranges::to<std::vector<Coord>>;

        // fmt::print("{}\n", ranges::views::all(data));

        {
            Point2 px {data.at(0), data.at(1)};
            Gfx_2d::Direction d {static_cast<int>(data.at(3)), static_cast<int>(data.at(4))};
            map2.push_back({std::move(px), std::move(d)});
        }
        {
            Point3 px {data.at(0), data.at(1), data.at(2)};
            Gfx_3d::Direction d {
                static_cast<int>(data.at(3)), static_cast<int>(data.at(4)), static_cast<int>(data.at(5))};
            map3.push_back({std::move(px), std::move(d)});
        }

    }

    part1(std::move(map2));

    return 0;
}
