#include "point2d.h"
#include "point3d.h"
#include "Mat.h"

#include <array>
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

using Coord = __int128_t;
using Point2 = Gfx_2d::Point<Coord>;
using Point3 = Gfx_3d::Point<Coord>;

using Storm2 = std::pair<Point2, Gfx_2d::Direction>;
using Storm3 = std::pair<Point3, Gfx_3d::Direction>;

using Map2d = std::vector<Storm2>;
using Map3d = std::vector<Storm3>;


void part1(Map2d map2)
{
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

            const Coord delitel = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (delitel == 0) {
                continue;
            }

            const Coord xh = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
            const Coord yh = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);

            const double ix = xh / static_cast<double>(delitel);
            const double iy = yh / static_cast<double>(delitel);

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

    fmt::print("1: {}\n", cnt);
}

void part2(Map3d map3)
{
    // thanks to https://github.com/DeadlyRedCube/AdventOfCode/blob/main/2023/AOC2023/D24.h
    const auto A0 {map3.at(0).first};
    const auto Av {map3.at(0).second};
    const auto B0 {map3.at(2).first};
    const auto Bv {map3.at(2).second};
    const auto C0 {map3.at(5).first};
    const auto Cv {map3.at(5).second};

    Mat<double, 6, 6> mat {{
        {0.0 + Av.dy - Bv.dy, 0.0 + Bv.dx - Av.dx, 0.0, 0.0 + B0.y - A0.y, 0.0 + A0.x - B0.x, 0.0},
        {0.0 + Av.dy - Cv.dy, 0.0 + Cv.dx - Av.dx, 0.0, 0.0 + C0.y - A0.y, 0.0 + A0.x - C0.x, 0.0},
        {0.0 + Bv.dz - Av.dz, 0.0, 0.0 + Av.dx - Bv.dx, 0.0 + A0.z - B0.z, 0.0, 0.0 + B0.x - A0.x},
        {0.0 + Cv.dz - Av.dz, 0.0, 0.0 + Av.dx - Cv.dx, 0.0 + A0.z - C0.z, 0.0, 0.0 + C0.x - A0.x},
        {0.0, 0.0 + Av.dz - Bv.dz, 0.0 + Bv.dy - Av.dy, 0.0, 0.0 + B0.z - A0.z, 0.0 + A0.y - B0.y},
        {0.0, 0.0 + Av.dz - Cv.dz, 0.0 + Cv.dy - Av.dy, 0.0, 0.0 + C0.z - A0.z, 0.0 + A0.y - C0.y},
    }};

    const std::array<double, 6> h = {
        0.0 + (B0.y * Bv.dx - B0.x * Bv.dy) - (A0.y * Av.dx - A0.x * Av.dy),
        0.0 + (C0.y * Cv.dx - C0.x * Cv.dy) - (A0.y * Av.dx - A0.x * Av.dy),
        0.0 + (B0.x * Bv.dz - B0.z * Bv.dx) - (A0.x * Av.dz - A0.z * Av.dx),
        0.0 + (C0.x * Cv.dz - C0.z * Cv.dx) - (A0.x * Av.dz - A0.z * Av.dx),
        0.0 + (B0.z * Bv.dy - B0.y * Bv.dz) - (A0.z * Av.dy - A0.y * Av.dz),
        0.0 + (C0.z * Cv.dy - C0.y * Cv.dz) - (A0.z * Av.dy - A0.y * Av.dz),
    };

    const auto inv = mat.Inverse();
    const auto res = inv * h;
    const auto sum {Coord(res.at(0) + res.at(1) + res.at(2))};

    fmt::print("2: {}\n", sum);
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
    part2(std::move(map3));

    return 0;
}
