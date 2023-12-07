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

    auto const sum = std::ranges::fold_left(
        getlines(std::cin) | std::views::transform(parse_coord), 0, std::plus<int>());

    std::cout << sum << '\n';
    return 0;
}
