#include <algorithm>
#include <iostream>
#include <numeric>
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

auto extend_parse(
    std::string::const_iterator begin, std::string::const_iterator iter,
    std::string::const_iterator end) -> int {
    auto const is_not_digit = [](char c) { return !std::isdigit(c); };
    auto const right = std::find_if(iter, end, is_not_digit);
    auto const left = std::find_if(
                          std::make_reverse_iterator(iter),
                          std::make_reverse_iterator(begin), is_not_digit)
                          .base();
    return std::stoi(std::string{left, right});
}

auto find_part_numbers(Line const& part_line, Line const& number_line)
    -> std::vector<int> {
    auto const is_part = [](char c) { return !std::isdigit(c) && c != '.'; };
    auto part_iter = std::find_if(part_line.begin(), part_line.end(), is_part);
    std::vector<int> numbers{};
    while (part_iter != part_line.end()) {
        auto const number_pos = std::distance(part_line.begin(), part_iter);
        auto number_iter = number_line.begin() + number_pos;
        // Find in the center, exclusive with corners
        if (std::isdigit(*number_iter)) {
            numbers.push_back(extend_parse(
                number_line.begin(), number_iter, number_line.end()));
        } else {
            // Find in the left corner
            if (number_iter != number_line.begin() &&
                std::isdigit(*std::prev(number_iter))) {
                numbers.push_back(extend_parse(
                    number_line.begin(), std::prev(number_iter),
                    number_line.end()));
            }
            // Find in the right corner
            if (std::next(number_iter) != number_line.end() &&
                std::isdigit(*std::next(number_iter))) {
                numbers.push_back(extend_parse(
                    number_line.begin(), std::next(number_iter),
                    number_line.end()));
            }
        }
        part_iter =
            std::find_if(std::next(part_iter), part_line.end(), is_part);
    }
    return numbers;
}

auto main() -> int {
    auto const lines = getlines(std::cin);
    auto sum = 0;
    for (auto i = lines.begin(); i != lines.end(); ++i) {
        // Compare previous to current
        if (i != lines.begin()) {
            auto numbers = find_part_numbers(*i, *std::prev(i));
            sum += std::accumulate(numbers.begin(), numbers.end(), 0);
        }
        // Find in the current
        {
            auto numbers = find_part_numbers(*i, *i);
            sum += std::accumulate(numbers.begin(), numbers.end(), 0);
        }
        // Compare to the next
        if (std::next(i) != lines.end()) {
            auto numbers = find_part_numbers(*i, *std::next(i));
            sum += std::accumulate(numbers.begin(), numbers.end(), 0);
        }
    }
    std::cout << sum << '\n';
    return 0;
}
