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
        std::cout << "    part" << *part_iter << std::endl;
        auto const number_pos = std::distance(part_line.begin(), part_iter);
        auto number_iter = number_line.begin() + number_pos;
        if (std::isdigit(*number_iter)) {
            std::cout << "      center" << std::endl;
            numbers.push_back(extend_parse(
                number_line.begin(), number_iter, number_line.end()));
        } else {
            if (number_iter != number_line.begin() &&
                std::isdigit(*std::prev(number_iter))) {
                std::cout << "      left" << std::endl;
                numbers.push_back(extend_parse(
                    number_line.begin(), std::prev(number_iter),
                    number_line.end()));
            }
            if (std::next(number_iter) != number_line.end() &&
                std::isdigit(*std::next(number_iter))) {
                std::cout << "      right" << std::endl;
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

    for (auto i = lines.begin(); i != lines.end(); ++i) {
        std::cout << "line #" << std::distance(lines.begin(), i) << std::endl;
        // Compare previous to current
        if (i != lines.begin()) {
            std::cout << "  compare to upper" << std::endl;
            // TODO
            for (auto x : find_part_numbers(*i, *std::prev(i))) {
                std::cout << "        " << x << "\n";
            }
        }
        // Find in the current
        {
            std::cout << "  compare to self" << std::endl;
            // TODO
        }
        // Compare to the next
        if (std::next(i) != lines.end()) {
            std::cout << "  compare to lower" << std::endl;
            // TODO
        }
    }

    return 0;
}
