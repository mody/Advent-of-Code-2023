#include <fmt/core.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/core.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <iostream>
#include <vector>

using Data = std::vector<int>;

int main()
{
    auto parse_line = []() -> Data {
        std::string line;
        std::getline(std::cin, line);
        assert(!line.empty());

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);

        using namespace ranges;
        return parts | views::drop(1) | views::transform([](auto&& val) { return std::stoi(val); }) | to<std::vector>;
    };

    Data times {parse_line()}, distances {parse_line()};

    auto try_race = [](uint64_t t, uint64_t d) {
        uint64_t solutions {0};
        for (uint64_t x {0}; x < t; ++x) {
            const uint64_t me = x * (t - x);
            if (me > d) ++solutions;
        }
        return solutions;
    };

    uint64_t result1 {1};
    for (auto const& [t, d] : ranges::views::zip(times, distances)) {
        result1 *= try_race(t, d);
    }

    fmt::print("1: {}\n", result1);

    auto merge_numbers = [](Data const& data) -> uint64_t {
        uint64_t result {0}, mul {1};
        for (uint64_t val : data | ranges::views::reverse) {
            result += val * mul;
            if (val < 10) {
                mul *= 10;
            } else if (val < 100) {
                mul *= 100;
            } else if (val < 1000) {
                mul *= 1000;
            } else if (val < 10000) {
                mul *= 10000;
            }
        }
        return result;
    };

    const uint64_t race_time {merge_numbers(times)}, race_distance {merge_numbers(distances)};

    fmt::print("2: {}\n", try_race(race_time, race_distance));

    return 0;
}
