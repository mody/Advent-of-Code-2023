#include <fmt/core.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/view/drop.hpp>

#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <vector>

using StrVect = std::vector<std::string>;

struct Part
{
    int x{0}, m{0}, a{0}, s{0};
};

using Parts = std::vector<Part>;

template<typename Op, int Part::*ptr>
struct Compare {
    explicit Compare(int v) noexcept
        : val {v}
    { }

    bool operator() (Part const& p) const noexcept {
        return Op{}(std::mem_fn(ptr)(p), val);
    }

    bool test(int v) const noexcept {
        return Op{}(v, val);
    }

    int val{0};
};

using X_is_lt = Compare<std::less<int>, &Part::x>;
using X_is_gt = Compare<std::greater<int>, &Part::x>;
using M_is_lt = Compare<std::less<int>, &Part::m>;
using M_is_gt = Compare<std::greater<int>, &Part::m>;
using A_is_lt = Compare<std::less<int>, &Part::a>;
using A_is_gt = Compare<std::greater<int>, &Part::a>;
using S_is_lt = Compare<std::less<int>, &Part::s>;
using S_is_gt = Compare<std::greater<int>, &Part::s>;

struct Condition {
    std::string dst;
    char val_id {' '};
    char op_id {' '};
    int val {0};
    std::function<bool(Part const&)> cmp;
};

using Conditions = std::vector<Condition>;

struct Rule {
    Conditions conditions;
};

using RuleMap = std::unordered_map<std::string, Rule>;

void read_block(std::string const& sep, auto&& op)
{
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        StrVect parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(sep), boost::token_compress_on);
        op(std::move(parts));
    }
}

std::optional<bool> apply_dst(RuleMap const& rules, Part const& part, std::string const& dst)
{
    if (dst == "A") {
        return true;
    } else if (dst == "R") {
        return false;
    }

    Rule const& rule {rules.at(dst)};
    for (auto const& cond : rule.conditions) {
        if (!cond.cmp || cond.cmp(part)) {
            return apply_dst(rules, part, cond.dst);
        }
    }

    return std::nullopt;
}

void part1(RuleMap const& rules, Parts const& parts)
{
    uint64_t sum{0};

    for (auto const& part : parts) {
        auto ret = apply_dst(rules, part, "in");
        if (!ret.has_value()) continue;
        if (ret.value()) {
            sum += part.x + part.m + part.a + part.s;
        }
    }
    fmt::print("1: {}\n", sum);
}

using RangePart = std::map<char, std::pair<int, int>>;

void check_dst(RuleMap const& rules, RangePart part, std::string const& dst, uint64_t& result)
{
    if (dst == "A") {
        result += std::accumulate(part.begin(), part.end(), 1ull, [](uint64_t a, auto const& r) {
            return a * (r.second.second - r.second.first + 1);
        });
        return;
    } else if (dst == "R") {
        return;
    }

    Rule const& rule {rules.at(dst)};
    for (auto const& cond : rule.conditions) {
        if (!cond.cmp) {
            check_dst(rules, part, cond.dst, result);
        } else {
            // split range
            // px{a<2006:qkq,m>2090:A,rfg}
            auto& r1 = part.at(cond.val_id);
            if (r1.first < cond.val && r1.second > cond.val) {
                // inside the interval
                RangePart part2{part};
                if (cond.op_id == '<') {
                    auto& r2 = part2.at(cond.val_id);
                    r2.second = cond.val - 1;
                    r1.first = cond.val;
                } else {
                    auto& r2 = part2.at(cond.val_id);
                    r2.first = cond.val + 1;
                    r1.second = cond.val;
                }
                assert(cond.cmp({part2.at('x').first, part2.at('m').first, part2.at('a').first, part2.at('s').first}));
                assert(
                    cond.cmp({part2.at('x').second, part2.at('m').second, part2.at('a').second, part2.at('s').second}));
                check_dst(rules, part2, cond.dst, result);
            } else {
                // outside the interval
            }
       }
    }
}

void part2(RuleMap const& rules)
{
    RangePart part {
        {'x', {1, 4000}},
        {'m', {1, 4000}},
        {'a', {1, 4000}},
        {'s', {1, 4000}},
    };

    uint64_t result{0};
    check_dst(rules, part, "in", result);

    fmt::print("2: {}\n", result);
}

int main()
{
    RuleMap rules;
    Parts parts;

    read_block("{},", [&rules](StrVect&& input) {
        // ["px", "a<2006:qkq", "m>2090:A", "rfg", ""]
        Rule& r = rules[input.at(0)];

        for (auto const& s : input | ranges::views::drop(1)) {
            if (s.empty()) break;
            const auto colon {s.find(':')};
            if (colon == std::string::npos) {
                // default rule, at the end
                r.conditions.push_back({.dst = s});
                break;
            }
            Condition c {.dst = s.substr(colon + 1)};
            c.val_id = s.at(0);
            c.op_id = s.at(1);
            std::string sval = {s.substr(2, colon - 2)};
            c.val = std::stoi(sval);
            switch(c.val_id) {
            case 'x': if(c.op_id == '<') c.cmp=X_is_lt{c.val}; else c.cmp=X_is_gt{c.val}; break;
            case 'm': if(c.op_id == '<') c.cmp=M_is_lt{c.val}; else c.cmp=M_is_gt{c.val}; break;
            case 'a': if(c.op_id == '<') c.cmp=A_is_lt{c.val}; else c.cmp=A_is_gt{c.val}; break;
            case 's': if(c.op_id == '<') c.cmp=S_is_lt{c.val}; else c.cmp=S_is_gt{c.val}; break;
            }
            r.conditions.push_back(std::move(c));
        }
    });

    read_block("{},=", [&parts](StrVect&& input) {
        // ["", "x", "787", "m", "2655", "a", "1222", "s", "2876", ""]
        parts.push_back(
            {std::stoi(input.at(2)), std::stoi(input.at(4)), std::stoi(input.at(6)), std::stoi(input.at(8))});
    });

    part1(rules, parts);
    part2(rules);

    return 0;
}
