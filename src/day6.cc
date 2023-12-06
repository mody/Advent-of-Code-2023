#include <fmt/core.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/core.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
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
        return parts | views::drop(1) | views::transform([](auto && val) { return std::stoi(val); }) | to<std::vector>;
    };

    Data times {parse_line()}, distances {parse_line()};

    uint64_t result1{1};
    for (auto const& [t, d] : ranges::views::zip(times, distances)) {
        int solutions{0};
        for (int x {0}; x < t; ++x) {
            const int me = x * (t - x);
            if (me > d) ++solutions;
        }
        // fmt::print("t: {}, d: {}, solutions: {}\n", t, d, solutions);
        result1 *= solutions;
    }

    fmt::print("1: {}\n", result1);

    return 0;
}
