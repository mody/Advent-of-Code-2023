#include <fmt/core.h>
#include <fmt/format.h>

#include <range/v3/action/sort.hpp>
#include <range/v3/view/cache1.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/enumerate.hpp>

#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

enum class Strenght {
    ONE = 1,
    PAIR,
    TWO_PAIR,
    THREE,
    FULL_HOUSE,
    FOUR,
    FIVE,
};

auto format_as(Strenght s) noexcept
{
    static const std::map<Strenght, std::string> conversion {
        {Strenght::ONE, "ONE"},
        {Strenght::PAIR, "PAIR"},
        {Strenght::TWO_PAIR, "TWO_PAIR"},
        {Strenght::THREE, "THREE"},
        {Strenght::FULL_HOUSE, "FULL_HOUSE"},
        {Strenght::FOUR, "FOUR"},
        {Strenght::FIVE, "FIVE"}};

    return fmt::format("{}", conversion.at(s));
}


enum class Card {
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

auto format_as(Cards cards) noexcept
{
    static const std::map<Card, unsigned char> CARD_TO_STR{
        {Card::_2, '2'},
        {Card::_3, '3'},
        {Card::_4, '4'},
        {Card::_5, '5'},
        {Card::_6, '6'},
        {Card::_7, '7'},
        {Card::_8, '8'},
        {Card::_9, '9'},
        {Card::_T, 'T'},
        {Card::_J, 'J'},
        {Card::_Q, 'Q'},
        {Card::_K, 'K'},
        {Card::_A, 'A'},
    };

    std::string s;
    s.reserve(5);
    for (auto c : cards){
        s.push_back(CARD_TO_STR.at(c));
    }
    return fmt::format("{}", s);
}

struct Deal
{
    unsigned bid{0};
    Cards cards;
};

using Game = std::vector<Deal>;


Strenght find_strenght(Cards cards) noexcept
{
    ranges::actions::sort(cards);

    std::vector<unsigned> result;
    result.reserve(2);

    Card c1 = cards.front();
    unsigned cnt{1};
    for (auto const& c2 : cards | ranges::views::drop(1)) {
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

    ranges::actions::sort(result);

    auto s{Strenght::ONE};

    if (result.size() == 2) {
        if (result.at(0) == 2 && result.at(1) == 2) {
            s = Strenght::TWO_PAIR;
        } else {
            s = Strenght::FULL_HOUSE;
        }
    } else if(result.size() == 1) {
        switch (result.at(0)) {
        case 2: s = Strenght::PAIR; break;
        case 3: s = Strenght::THREE; break;
        case 4: s = Strenght::FOUR; break;
        case 5: s = Strenght::FIVE; break;
        }
    }

    return s;
}

void part1(Game game) noexcept
{
    auto cmp = [](Deal const& lhs, Deal const& rhs) noexcept {
        const auto s1 = find_strenght(lhs.cards);
        const auto s2 = find_strenght(rhs.cards);
        if (s1 == s2) {
            return lhs.cards < rhs.cards;
        }
        return s1 < s2;
    };

    ranges::actions::sort(game, cmp);

    uint64_t result{0};
    for (auto const& [idx, d] : ranges::views::enumerate(game)) {
        result += (idx + 1) * d.bid;
    }

    fmt::print("1: {}\n", result);
}

int main()
{
    Game game;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        static const std::map<unsigned char, Card> TO_CARD{
            {'2', Card::_2},
            {'3', Card::_3},
            {'4', Card::_4},
            {'5', Card::_5},
            {'6', Card::_6},
            {'7', Card::_7},
            {'8', Card::_8},
            {'9', Card::_9},
            {'T', Card::_T},
            {'J', Card::_J},
            {'Q', Card::_Q},
            {'K', Card::_K},
            {'A', Card::_A},
        };

        Cards cards{};

        auto in = std::string_view{line};
        for (auto const& [idx, c] : in.substr(0, 5) | ranges::views::enumerate) {
            cards.at(idx) = TO_CARD.at(c);
        }

        game.push_back(Deal {.bid = static_cast<unsigned int>(std::stoul(line.substr(6))), .cards = std::move(cards)});
    }

    part1(game);

    return 0;
}
