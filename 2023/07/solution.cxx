#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <vector>

auto card_to_int(char card) -> int {
    return card == 'A' ? 14 :
           card == 'K' ? 13 :
           card == 'Q' ? 12 :
           card == 'J' ? 11 :
           card == 'T' ? 10 :
                         card - '0';
}

using Cards = std::array<int, 5>;
struct Hand {
    Cards cards;
    int bet;
};
using Hands = std::vector<Hand>;

/*
Adding jokers to the hand

AAAAA/
 * No jokers possible
AAAA/B
 * AAAAJ -> AAAAA -> #A+1
AAABB/
 * No jokers possible
AAA/BC
 * AAAJB -> AAAAB -> #A+1
 * AAAJJ -> AAAAA -> #A+2
AABB/C
 * AABBJ -> AAABB -> #A+1
AA/BCD
 * AAJBC -> AAABC -> #A+1
 * AAJJB -> AAAAB -> #A+2
 * AAJJJ -> AAAAA -> #A+3
A/BCDE
 * AJBCD -> AABCD -> #A+1
 * AJJBC -> AAABC -> #A+2
 * AJJJB -> AAAAB -> #A+3
 * AJJJJ -> AAAAA -> #A+4

Summary: add #jokers to the top count

Plus special case all JJJJJ
*/

auto rate_hand(Hand const& hand, bool joker) -> int {
    std::map<int, int> mapping;
    auto jokers = 0;
    for (auto const& card : hand.cards) {
        if (joker && card == 11) {
            ++jokers;
        } else {
            ++mapping[card];
        }
    }
    std::vector<int> powers;
    for (auto const& [card, count] : mapping) {
        powers.push_back(count);
    }
    std::sort(powers.rbegin(), powers.rend());
    if (powers.empty()) {
        powers.push_back(0);
    }
    powers[0] += jokers;
    auto p = powers.size() == 0 || powers[0] == 5 ? 7 :
             powers[0] == 4                       ? 6 :
             powers[0] == 3 && powers[1] == 2     ? 5 :
             powers[0] == 3 && powers[1] == 1     ? 4 :
             powers[0] == 2 && powers[1] == 2     ? 3 :
             powers[0] == 2 && powers[1] == 1     ? 2 :
                                                    1;
    return p;
}
auto rate_hand_tiebreaker(Hand const& hand, int jvalue) -> int {
    const auto base = 14;
    auto checksum = 0;
    for (auto card : hand.cards) {
        checksum = checksum * base + (card == 11 ? jvalue : card);
    }
    return checksum;
}

auto compare(Hand const& a, Hand const& b, int jvalue, bool joker) -> bool {
    auto const a_rating = rate_hand(a, joker);
    auto const b_rating = rate_hand(b, joker);
    return a_rating < b_rating ? true :
           a_rating > b_rating ? false :
                                 rate_hand_tiebreaker(a, jvalue) <
                                     rate_hand_tiebreaker(b, jvalue);
}

auto joker_aware_compare(Hand const& a, Hand const& b) -> bool {
    return compare(a, b, 1, true);
}

auto operator<(Hand const& a, Hand const& b) -> bool {
    return compare(a, b, 11, false);
}

auto debug_print(Hand const& hand) -> void {
    std::cout << "(HAND:";
    for (auto const& card : hand.cards) {
        std::cout << " " << card;
    }
    std::cout << ") (BET: " << hand.bet << ")\n";
}

auto debug_print(Hands const& hands) -> void {
    for (auto const& hand : hands) {
        debug_print(hand);
    }
}

auto parse(std::istream& input) -> Hands {
    Hands hands;
    std::string line;
    while (std::getline(input, line)) {
        Cards cards{};
        for (auto i = line.begin(); i < line.begin() + 5; ++i) {
            cards[std::distance(line.begin(), i)] = card_to_int(*i);
        }
        auto const bet = std::stoi(std::string(line.begin() + 6, line.end()));
        hands.emplace_back(cards, bet);
    }
    return hands;
}

auto rate(Hands const& hands) -> int {
    return std::ranges::fold_left(
        std::views::transform(
            hands, [](Hand const& hand) { return hand.bet; }) |
            std::views::enumerate | std::views::transform([](auto const& kv) {
                return (std::get<0>(kv) + 1) * std::get<1>(kv);
            }),
        0, std::plus{});
}

auto main() -> int {
    auto hands = parse(std::cin);
    std::sort(hands.begin(), hands.end());
    std::cout << rate(hands) << std::endl;
    std::sort(hands.begin(), hands.end(), joker_aware_compare);
    debug_print(hands);
    std::cout << rate(hands) << std::endl;
    return 0;
}
