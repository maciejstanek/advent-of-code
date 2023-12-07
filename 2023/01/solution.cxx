#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using Line = std::string;
using Lines = std::vector<Line>;

auto getlines(std::istream& input) -> Lines {
    Lines lines;
    Line line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    return lines;
}

auto main() -> int {
    auto const parse_coord = [](std::string const& line) {
        auto const base = 10;
        auto const isdigit = [](char x) { return std::isdigit(x); };
        auto const first = static_cast<int>(*std::ranges::find_if(line, isdigit) - '0');
        auto const last =
            static_cast<int>(*std::ranges::find_if(line | std::views::reverse, isdigit) - '0');
        return base * first + last;
    };

    auto const lines = getlines(std::cin);
    auto const sum =
        std::ranges::fold_left(lines | std::views::transform(parse_coord), 0, std::plus<int>());
    std::cout << sum << '\n';

    auto const tokens = std::vector<std::string>{
        "0",   "1",   "2",     "3",    "4",    "5",   "6",     "7",     "8",   "9",
        "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    auto const reverse = [](std::string const& value) -> std::string { return {value.rbegin(), value.rend()}; };
    auto const reversed_tokens_view = tokens | std::views::transform(reverse);
    auto const reversed_tokens = std::vector<std::string>{
        std::ranges::begin(reversed_tokens_view),
        std::ranges::end(reversed_tokens_view)};

    // TODO: How to change parameter type to accept both lists?
    //       auto const reversed_tokens = tokens | std::views::transform(reverse);
    auto const parse_extended_coord = [](decltype(tokens) const& tokens) {
        return [&tokens](std::string const& line) {
            auto const find_position = [&line](std::string const& token) {
                return line.find(token);
            };
            auto found_view = tokens | std::views::transform(find_position);
            auto const min_elem = std::ranges::min_element(found_view);
            auto const pos = std::ranges::distance(std::ranges::begin(found_view), min_elem);
            return pos > 9 ? pos - 9 : pos + 1;
        };
    };

    auto firsts = lines | std::views::transform(parse_extended_coord(tokens));
    auto lasts = lines | std::views::transform(reverse) | std::views::transform(parse_extended_coord(reversed_tokens));

    auto const firsts_sum = std::ranges::fold_left(firsts, 0, std::plus<int>());
    auto const lasts_sum = std::ranges::fold_left(lasts, 0, std::plus<int>());
    std::cout << 10 * firsts_sum + lasts_sum << '\n';

    return 0;
}
