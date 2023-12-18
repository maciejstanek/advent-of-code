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

auto rate_hand(Hand const& hand) -> int {
    std::map<int, int> mapping;
    std::cout << "rate_hand:";
    for (auto x : hand.cards) {
        std::cout << " " << x;
    }
    std::cout << "\n";
    for (auto const& card : hand.cards) {
        ++mapping[card];
    }
    std::vector<int> powers;
    std::cout << " * powers:";
    for (auto const& [card, count] : mapping) {
        powers.push_back(count);
        std::cout << " (" << card << " #" << count << ")";
    }
    std::cout << "\n";
    std::sort(powers.rbegin(), powers.rend());
    std::cout << " * sorted powers:";
    for (auto x : powers) {
        std::cout << " " << x;
    }
    std::cout << "\n";
    auto p = powers[0] == 5                   ? 7 :
             powers[0] == 4                   ? 6 :
             powers[0] == 3 && powers[1] == 2 ? 5 :
             powers[0] == 3 && powers[1] == 1 ? 4 :
             powers[0] == 2 && powers[1] == 2 ? 3 :
             powers[0] == 2 && powers[1] == 1 ? 2 :
                                                1;
    std::cout << " * points " << p << "\n";
    return p;
}
auto rate_hand_tiebreaker(Hand const& hand) -> int {
    const auto base = 14;
    auto checksum = 0;
    for (auto card : hand.cards) {
        checksum = checksum * base + card;
    }
    return checksum;
}

auto operator<(Hand const& a, Hand const& b) -> bool {
    auto const a_rating = rate_hand(a);
    auto const b_rating = rate_hand(b);
    return a_rating < b_rating ? true :
           a_rating > b_rating ? false :
                                 rate_hand_tiebreaker(a) <
                                     rate_hand_tiebreaker(b);
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

auto main() -> int {
    auto hands = parse(std::cin);
    std::sort(hands.begin(), hands.end());
    debug_print(hands);
    auto sum = std::ranges::fold_left(
        std::views::transform(
            hands, [](Hand const& hand) { return hand.bet; }) |
            std::views::enumerate | std::views::transform([](auto const& kv) {
                return (std::get<0>(kv) + 1) * std::get<1>(kv);
            }),
        0, std::plus{});
    std::cout << sum << std::endl;

    return 0;
}
