#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/action/reverse.hpp>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/sliding.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

using Sequence = std::vector<int64_t>;
using Sequences = std::vector<Sequence>;

using namespace ranges;

template<typename T> class TD;

void part1(Sequences const& input)
{
    int64_t sum {0};

    for (Sequence x : input) {
        actions::reverse(x);

        Sequences tmp;
        tmp.push_back(x);

        for (;;) {
            Sequence y;
            int64_t a = x.front();
            for (auto b : x | views::drop(1)) {
                y.push_back(a - b);
                a = b;
            }
            if (all_of(y, [](int64_t v) { return v == 0; })) {
                break;
            }
            tmp.push_back(y);
            std::swap(x, y);
        }

        actions::reverse(tmp);

        int64_t val{0};
        for (auto const& s : tmp) {
            val += s.front();
        }
        sum += val;

    }
    fmt::print("1: {}\n", sum);
}

void part2(Sequences const& input)
{
    int64_t sum {0};

    for (Sequence x : input) {

        Sequences tmp;
        tmp.push_back(x);

        for (;;) {
            Sequence y;
            int64_t a = x.front();
            for (auto b : x | views::drop(1)) {
                y.push_back(b - a);
                a = b;
            }
            if (all_of(y, [](int64_t v) { return v == 0; })) {
                break;
            }
            tmp.push_back(y);
            std::swap(x, y);
        }

        actions::reverse(tmp);

        int64_t val{0};
        for (auto const& s : tmp) {
            val = s.front() - val;
        }
        sum += val;

    }
    fmt::print("2: {}\n", sum);
}

int main()
{
    Sequences input;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        Sequence sequence;

        std::stringstream ss(line);
        std::transform(
            std::istream_iterator<std::string>(ss), {}, std::back_insert_iterator<Sequence> {sequence},
            [](std::string const& val) -> int64_t { return std::stoll(val); });

        input.push_back(std::move(sequence));
    }

    part1(input);
    part2(input);

    return 0;
}
