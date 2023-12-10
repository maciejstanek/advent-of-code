#include <algorithm>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

using Number = int;
using Numbers = std::vector<Number>;
struct Card {
    Numbers winners;
    Numbers numbers;
};
using Cards = std::vector<Card>;

auto parse_numbers(
    std::string::const_iterator begin, std::string::const_iterator end)
    -> Numbers {
    Numbers numbers;
    auto const is_number = [](char c) { return std::isdigit(c); };
    auto const is_space = [](char c) { return c == ' '; };
    auto current = std::find_if(begin, end, is_number);
    while (current != end) {
        auto space = std::find_if(current, end, is_space);
        numbers.push_back(std::stoi(std::string{current, space}));
        current = std::find_if(space, end, is_number);
    }
    return numbers;
}

auto parse_cards(std::istream& input) -> Cards {
    Cards cards;
    std::string line;
    while (std::getline(input, line)) {
        auto const start = std::find(line.begin(), line.end(), ':') + 1;
        auto const divider = std::find(start, line.end(), '|');
        auto const winners = parse_numbers(start, divider);
        auto const numbers = parse_numbers(std::next(divider), line.end());
        cards.emplace_back(winners, numbers);
    }
    return cards;
}

auto get_points_for_cards_number(int n) -> int {
    if (!n) {
        return 0;
    }
    auto result = 1;
    while (--n) {
        result *= 2;
    }
    return result;
}

auto calculate_matching_count(Card const& card) -> int {
    auto winners = card.winners;
    std::sort(winners.begin(), winners.end());
    auto numbers = card.numbers;
    std::sort(numbers.begin(), numbers.end());
    Numbers intersection;
    std::set_intersection(
        winners.begin(), winners.end(), numbers.begin(), numbers.end(),
        std::back_inserter(intersection));
    return intersection.size();
}

auto calculate_points(Cards const& cards) -> int {
    auto const transform = [](auto const& x) {
        return get_points_for_cards_number(calculate_matching_count(x));
    };
    return std::transform_reduce(
        cards.begin(), cards.end(), 0, std::plus<>(), transform);
}

auto main() -> int {
    auto const cards = parse_cards(std::cin);
    auto const points = calculate_points(cards);
    std::cout << points << "\n";
    return 0;
}
