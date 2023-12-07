#include <fmt/core.h>
#include <fmt/format.h>

#include <range/v3/action/sort.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>

#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace ranges;

enum class Strenght
{
    ONE = 1,
    PAIR,
    TWO_PAIR,
    THREE,
    FULL_HOUSE,
    FOUR,
    FIVE,
};

enum class Card
{
    _X = 1,
    _2 = 2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    _T,
    _J,
    _Q,
    _K,
    _A,
};

using Cards = std::array<Card, 5>;

struct Deal
{
    unsigned bid {0};
    Cards cards;
};

using Game = std::vector<Deal>;


Strenght find_strength1(Cards cards) noexcept
{
    actions::sort(cards);

    std::vector<unsigned> result;
    result.reserve(2);

    Card c1 = cards.front();
    unsigned cnt {1};
    for (auto const& c2 : cards | views::drop(1)) {
        if (c1 == c2) {
            ++cnt;
        } else {
            if (cnt > 1) {
                result.push_back(cnt);
            }
            cnt = 1;
        }
        c1 = c2;
    }
    if (cnt > 1) {
        result.push_back(cnt);
    }

    actions::sort(result);

    auto s {Strenght::ONE};

    if (result.size() == 2) {
        if (result.at(0) == 2 && result.at(1) == 2) {
            s = Strenght::TWO_PAIR;
        } else {
            s = Strenght::FULL_HOUSE;
        }
    } else if (result.size() == 1) {
        switch (result.at(0)) {
        case 2: s = Strenght::PAIR; break;
        case 3: s = Strenght::THREE; break;
        case 4: s = Strenght::FOUR; break;
        case 5: s = Strenght::FIVE; break;
        }
    }

    return s;
}

uint64_t process(Game game, auto&& cmp) noexcept
{
    actions::sort(game, cmp);

    uint64_t result {0};
    for (auto const& [idx, d] : views::enumerate(game)) {
        result += (idx + 1) * d.bid;
    }

    return result;
}

Strenght find_strength2(Cards cards) noexcept
{
    std::map<Card, unsigned> counts;
    for (auto c : cards) {
        if (c != Card::_X) counts.insert({c, 0}).first->second++;
    }

    if (counts.empty()) {
        // all Js
        return Strenght::FIVE;
    }

    auto x = counts | views::transform([](auto&& v) { return std::make_pair(v.second, v.first); })
        | to_vector;
    actions::sort(x, [](const auto& lhs, const auto& rhs) {
        return (lhs.first == rhs.first) ? lhs.second > rhs.second : lhs.first > rhs.first;
    });

    const Card as_J = x.front().second;

    for (auto& c : cards) {
        if (c == Card::_X) {
            c = as_J;
        }
    }

    return find_strength1(cards);
}

void part1(Game game) noexcept
{
    auto cmp = [](Deal const& lhs, Deal const& rhs) noexcept {
        const auto s1 = find_strength1(lhs.cards);
        const auto s2 = find_strength1(rhs.cards);
        if (s1 == s2) {
            return lhs.cards < rhs.cards;
        }
        return s1 < s2;
    };

    fmt::print("1: {}\n", process(std::move(game), cmp));
}

void part2(Game game) noexcept
{
    for (auto& g : game) {
        for (auto& c : g.cards) {
            if (c == Card::_J) c = Card::_X;
        }
    }

    auto cmp = [](Deal const& lhs, Deal const& rhs) noexcept {
        const auto s1 = find_strength2(lhs.cards);
        const auto s2 = find_strength2(rhs.cards);
        if (s1 == s2) {
            return lhs.cards < rhs.cards;
        }
        return s1 < s2;
    };

    fmt::print("2: {}\n", process(std::move(game), cmp));
}

int main()
{
    Game game;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        static const std::map<unsigned char, Card> TO_CARD {
            {'2', Card::_2}, {'3', Card::_3}, {'4', Card::_4}, {'5', Card::_5}, {'6', Card::_6},
            {'7', Card::_7}, {'8', Card::_8}, {'9', Card::_9}, {'T', Card::_T}, {'J', Card::_J},
            {'Q', Card::_Q}, {'K', Card::_K}, {'A', Card::_A},
        };

        Cards cards {};

        for (auto const& [idx, c] : line | views::take(5) | views::enumerate) {
            cards.at(idx) = TO_CARD.at(c);
        }

        game.push_back(Deal {.bid = static_cast<unsigned int>(std::stoul(line.substr(6))), .cards = std::move(cards)});
    }

    part1(game);
    part2(game);

    return 0;
}
