#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>

#include <bitset>
#include <iostream>
#include <sstream>
#include <vector>

using Counts = std::vector<unsigned>;

struct Row
{
    std::string springs;
    Counts counts;
};

using Garden = std::vector<Row>;

struct Processor
{
    Processor(Counts c)
        : counts {std::move(c)}
    { }

    void operator() (std::string mask);

    unsigned options{0};
    const Counts counts;
};

    void Processor::operator()(std::string mask)
{
    const auto from = mask.find('?');
    if (from == std::string::npos) {
        // check counts
        Counts cts;
        unsigned count{0};
        for (auto c : mask) {
            if (c == '#') {
                ++count;
            } else {
                if (count) {
                    cts.push_back(count);
                }
                count = 0;
            }
        }
        if (count) {
            cts.push_back(count);
        }

        if (cts == counts) {
            ++options;
        }
    } else {
        // generate
        const unsigned bits = [&]() -> unsigned {
            const auto end = mask.find_first_not_of('?', from);
            if (end == std::string::npos)
                return 1;
            else
                return end - from;
        }();
        const unsigned bitmask {(1u << bits) - 1};
        std::bitset<16> val {0};
        do {
            for (unsigned idx {0}; idx < bits; ++idx) {
                mask.at(from + idx) = val.test(idx) ? '#' : '.';
            }

            this->operator()(mask);

            val = (val.to_ulong() + 1) & bitmask;
        } while (val != 0);
    }
}

void part1(Garden const& garden)
{
    unsigned sum{0};
    for (auto const& row : garden) {
        Processor p {row.counts};
        p(row.springs);
        sum += p.options;
    }
    fmt::print("1: {}\n", sum);
}

int main()
{
    Garden garden;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        Row r;

        auto split = line.find(' ');
        r.springs = line.substr(0, split);

        std::vector<std::string> parts;
        boost::algorithm::split(
            parts, line.substr(split + 1), boost::algorithm::is_any_of(","), boost::token_compress_on);
        r.counts = parts | ranges::views::transform([](auto&& v) { return std::stoll(v); })
            | ranges::to<std::vector<unsigned>>;

        garden.push_back(std::move(r));
    }

    part1(garden);

    return 0;
}
