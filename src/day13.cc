#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/all.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/enumerate.hpp>

#include <iostream>
#include <string_view>
#include <vector>

using Line = std::string;
using Map = std::vector<Line>;
using Maps = std::vector<Map>;

using namespace ranges;

void part1(Maps maps)
{
    auto find_mirror = [](Map const& m) -> unsigned {
        std::vector<unsigned> mirrors;
        {
            unsigned mirror {1};
            std::string_view l1 = m.front();
            for (std::string_view l2 : m | views::drop(1)) {
                if (l1 == l2) {
                    mirrors.push_back(mirror);
                }
                std::swap(l1, l2);
                ++mirror;
            }
        }

        for (auto const mirror : mirrors) {
            bool ok{true};
            for (unsigned i1 = mirror, i2 = mirror + 1; i1 > 0 && i2 <= m.size(); --i1, ++i2) {
                if (m.at(i1 - 1) != m.at(i2 - 1)) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return mirror;
            }
        }
        return 0;
    };

    unsigned sum {0};

    for (auto const& map : maps) {
        unsigned horizontal = find_mirror(map);
        // fmt::print("horizontal: {}\n", horizontal);
        if (horizontal) {
            sum += 100 * horizontal;
            continue;
        }

        Map m2 {map.front().size()};
        for (Line const& line : map) {
            for (auto [idx, c] : line | views::enumerate) {
                m2.at(idx).push_back(c);
            }
        }

        unsigned vertical = find_mirror(m2);
        // fmt::print("vertical: {}\n", vertical);
        sum += vertical;
    }

    fmt::print("1: {}\n", sum);
}

int main()
{
    Maps maps;
    Map map;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            maps.push_back(std::move(map));
            map.clear();
            continue;
        }

        map.push_back(std::move(line));
    }
    if (!map.empty()) maps.push_back(std::move(map));

    part1(maps);

    return 0;
}
