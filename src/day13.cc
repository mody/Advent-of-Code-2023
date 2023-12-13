#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/all.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/zip.hpp>

#include <iostream>
#include <string_view>
#include <vector>

using Line = std::string;
using Map = std::vector<Line>;
using Maps = std::vector<Map>;

using namespace ranges;

std::pair<bool, unsigned> compare_with_error(auto const& lhs, auto const& rhs)
{
    unsigned diff {0};
    for (auto const p : views::zip(lhs, rhs)) {
        if (p.first != p.second) {
            ++diff;
        }
    }
    return {diff <= 1, diff};
}

std::vector<unsigned> find_mirrors(Map const& map)
{
    std::vector<unsigned> mirrors;
    unsigned mirror {1};
    std::string_view l1 = map.front();
    for (std::string_view l2 : map | views::drop(1)) {
        if (l1 == l2) {
            mirrors.push_back(mirror);
        }
        std::swap(l1, l2);
        ++mirror;
    }
    return mirrors;
}

template<typename T>
void find_dirty_mirrors(Map const& map, T&& fn)
{
    unsigned mirror {1};
    std::string_view l1 = map.front();
    for (std::string_view l2 : map | views::drop(1)) {
        auto p = compare_with_error(l1, l2);
        if (p.first) {
            fn(mirror);
        }
        std::swap(l1, l2);
        ++mirror;
    }
}

unsigned find_mirror(Map const& map)
{
    auto mirrors = find_mirrors(map);

    for (auto const& mirror : mirrors) {
        bool ok {true};
        for (unsigned i1 = mirror, i2 = mirror + 1; i1 > 0 && i2 <= map.size(); --i1, ++i2) {
            if (map.at(i1 - 1) != map.at(i2 - 1)) {
                ok = false;
                break;
            }
        }
        if (ok) {
            return mirror;
        }
    }
    return 0;
}


void part1(Maps const& maps)
{
    unsigned sum {0};

    for (auto const& map : maps) {
        unsigned horizontal = find_mirror(map);
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
        sum += vertical;
    }

    fmt::print("1: {}\n", sum);
}

void part2(Maps const& maps)
{
    unsigned sum {0};

    for (auto const& map : maps) {
        std::vector<unsigned> mirrors;
        find_dirty_mirrors(map, [&map, &mirrors](unsigned mirror) {
            unsigned diff {0};
            for (unsigned i1 = mirror, i2 = mirror + 1; i1 > 0 && i2 <= map.size(); --i1, ++i2) {
                const auto p = compare_with_error(map.at(i1 - 1), map.at(i2 - 1));
                if (!p.first || p.second+diff > 1) {
                    return;
                }
                diff += p.second;
            }
            if (diff == 1) {
                mirrors.push_back(mirror);
            }
        });

        if (!mirrors.empty()) {
            sum += 100 * mirrors.front();
            continue;
        }

        Map m2 {map.front().size()};
        for (Line const& line : map) {
            for (auto [idx, c] : line | views::enumerate) {
                m2.at(idx).push_back(c);
            }
        }

        find_dirty_mirrors(m2, [&m2, &mirrors](unsigned mirror) {
            unsigned diff = 0;
            for (unsigned i1 = mirror, i2 = mirror + 1; i1 > 0 && i2 <= m2.size(); --i1, ++i2) {
                const auto p = compare_with_error(m2.at(i1 - 1), m2.at(i2 - 1));
                if (!p.first || p.second+diff > 1) {
                    return;
                }
                diff += p.second;
            }
            if (diff == 1) {
                mirrors.push_back(mirror);
            }
        });

        if (!mirrors.empty()) {
            sum += mirrors.front();
        }
    }

    fmt::print("2: {}\n", sum);

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
    part2(maps);

    return 0;
}
