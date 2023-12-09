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

void process(Sequences const& input)
{
    int64_t sum1 {0}, sum2 {0};

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

        int64_t val1 {0}, val2 {0};
        for (auto const& s : tmp) {
            val1 += s.front();
            val2 = s.back() - val2;
        }
        sum1 += val1;
        sum2 += val2;

    }
    fmt::print("1: {}\n", sum1);
    fmt::print("2: {}\n", sum2);
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

    process(input);

    return 0;
}
