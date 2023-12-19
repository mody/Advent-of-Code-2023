#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/view/all.hpp>
#include <range/v3/view/drop.hpp>

#include <any>
#include <functional>
#include <iostream>
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
    Compare(int v) noexcept
        : val {v}
    { }

    bool operator() (Part const& p) const noexcept {
        return Op{}(std::mem_fn(ptr)(p), val);
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
            const char val_id {s.at(0)};
            const char op_id {s.at(1)};
            std::string sval = {s.substr(2, colon - 2)};
            const int val {std::stoi(sval)};
            Condition c {.dst = s.substr(colon + 1)};
            switch(val_id) {
            case 'x': if(op_id == '<') c.cmp=X_is_lt{val}; else c.cmp=X_is_gt{val}; break;
            case 'm': if(op_id == '<') c.cmp=M_is_lt{val}; else c.cmp=M_is_gt{val}; break;
            case 'a': if(op_id == '<') c.cmp=A_is_lt{val}; else c.cmp=A_is_gt{val}; break;
            case 's': if(op_id == '<') c.cmp=S_is_lt{val}; else c.cmp=S_is_gt{val}; break;
            }
            r.conditions.push_back(std::move(c));
        }
    });

    read_block("{},=", [&parts](StrVect&& input) {
        // ["", "x", "787", "m", "2655", "a", "1222", "s", "2876", ""]
        parts.push_back(
            {std::stoi(input.at(2)), std::stoi(input.at(4)), std::stoi(input.at(6)), std::stoi(input.at(8))});
    });

    // for(auto const& p : parts) {
    //     fmt::print("{{x={},m={},a={},s={}}}\n", p.x, p.m, p.a, p.s);
    // }

    part1(rules, parts);

    return 0;
}
