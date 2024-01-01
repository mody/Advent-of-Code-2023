#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/transform.hpp>

#include <iostream>
#include <vector>

using Coord = __int128_t;
struct Storm3 {
  Coord a{0}, b{0}, c{0};
  Coord sx{0}, sy{0}, sz{0};
  Coord vx{0}, vy{0}, vz{0};
};
using Map3d = std::vector<Storm3>;


void part1(Map3d map3)
{
    // constexpr Coord MIN_X{7}, MAX_X{27};
    // constexpr Coord MIN_Y{7}, MAX_Y{27};
    constexpr Coord MIN_X{200000000000000}, MAX_X{400000000000000};
    constexpr Coord MIN_Y{200000000000000}, MAX_Y{400000000000000};

    unsigned cnt{0};

    for (auto i1 = map3.begin(); i1 != map3.end(); ++i1) {
        auto const& a1 = i1->a;
        auto const& b1 = i1->b;
        auto const& c1 = i1->c;
        for (auto i2 = std::next(i1); i2 != map3.end(); ++i2) {
            auto const& a2 = i2->a;
            auto const& b2 = i2->b;
            auto const& c2 = i2->c;
            if (a1 * b2 == b1 * a2) {
                continue;
            }
            const auto x = (c1 * b2 - c2 * b1) / (a1 * b2 - a2 * b1);
            const auto y = (c2 * a1 - c1 * a2) / (a1 * b2 - a2 * b1);
            if (x >= MIN_X && x <= MAX_X && y >= MIN_Y && y <= MAX_Y) {
                if ((x - i1->sx) * i1->vx >= 0 && (y - i1->sy) * i1->vy >= 0
                && (x - i2->sx) * i2->vx >= 0 && (y - i2->sy) * i2->vy >= 0) {
                    ++cnt;
                }
            }
        }
    }

    fmt::print("1: {}\n", cnt);
}

int main()
{
    Map3d map3;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" ,@"), boost::token_compress_on);

        Storm3 s{0};
        auto&& data = parts
            | ranges::views::transform(([](auto&& val) { return std::stoll(val); }))
            | ranges::to_vector;
        std::tie(s.sx, s.sy, s.sz, s.vx, s.vy, s.vz) =
            std::tie(data.at(0), data.at(1), data.at(2), data.at(3), data.at(4), data.at(5));
        s.a = data.at(4);
        s.b = -data.at(3);
        s.c = data.at(4) * data.at(0) - data.at(3) * data.at(1);
        map3.push_back(std::move(s));
    }

    part1(map3);

    return 0;
}
