#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

using Line = std::string;
using Lines = std::vector<Line>;

auto parse_lines(std::istream& input) -> Lines {
    Lines lines;
    Line line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    return lines;
}

auto parse_numbers(Line::const_iterator begin, Line::const_iterator end)
    -> std::vector<int> {
    std::vector<int> numbers;
    auto const is_digit = [](char c) { return std::isdigit(c); };
    auto current = std::find_if(begin, end, is_digit);
    while (current != end) {
        auto current_end = std::find_if_not(current, end, is_digit);
        numbers.push_back(std::stoi(std::string(current, current_end)));
        current = std::find_if(current_end, end, is_digit);
    }
    return numbers;
}

struct Span {
    int to;
    int from;
    int length;
};
struct Map {
    std::string to;
    std::vector<Span> entries;
};
struct Almanac {
    std::vector<int> seeds;
    std::map<std::string, Map> map;
};

auto parse_seeds(Line const& line) -> std::vector<int> {
    auto const seeds_begin = std::find(line.begin(), line.end(), ':');
    assert(seeds_begin != line.end());
    return parse_numbers(seeds_begin, line.end());
}

auto parse_map(Lines::const_iterator begin, Lines::const_iterator end)
    -> std::pair<std::string, Map> {
    assert(begin != end);
    auto const& header = *begin;
    auto const first_dash = std::find(header.begin(), header.end(), '-');
    auto const second_dash =
        std::find(std::next(first_dash), header.end(), '-');
    auto const space = std::find(second_dash, header.end(), ' ');
    auto const from = std::string(header.begin(), first_dash);
    auto const to = std::string(std::next(second_dash), space);

    std::vector<Span> entries;
    ++begin;
    while (begin != end) {
        auto values = parse_numbers(begin->begin(), begin->end());
        assert(values.size() == 3);
        entries.emplace_back(values[0], values[1], values[2]);
        ++begin;
    }

    return std::make_pair(from, Map{to, entries});
}

auto interpret_lines(std::vector<std::string> const& lines) -> Almanac {
    Almanac almanac{};
    auto current = lines.begin();
    while (current != lines.end()) {
        if (current->starts_with("seeds:")) {
            almanac.seeds = parse_seeds(*current);
            ++current;
        } else if (current->find("map:") != std::string::npos) {
            auto const is_empty = [](std::string const& line) {
                return line.empty();
            };
            auto const end = find_if(current, lines.end(), is_empty);
            almanac.map.emplace(parse_map(current, end));
            current = end;
        } else if (current->empty()) {
            ++current;
        }
    }
    return almanac;
}

auto print(std::ostream& out, Almanac const& almanac) -> void {
    out << "SEEDS:";
    for (auto x : almanac.seeds) {
        out << " " << x;
    }
    out << '\n';
    for (auto const& [x, y] : almanac.map) {
        out << "FROM " << x;
        out << " TO " << y.to << "\n";
        for (auto const& z : y.entries) {
            out << "  [" << z.from << ":" << z.from + z.length - 1
                      << "] -> [" << z.to << ":" << z.to + z.length - 1
                      << "]\n";
        }
    }
}

auto main() -> int {
    auto const almanac = interpret_lines(parse_lines(std::cin));
    print(std::cout, almanac);
    return 0;
}
