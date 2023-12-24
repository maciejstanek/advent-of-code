#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using Line = std::string;
using Input = std::vector<Line>;
using Number = int;
using Numbers = std::vector<int>;

auto parse(std::istream& input) -> Input {
    Input result;
    Line line;
    while (std::getline(input, line)) {
        result.push_back(line);
    }
    return result;
}

auto invert(Input const& input) -> Input {
    Input result(input[0].size());
    for (auto i = input.begin(); i != input.end(); ++i) {
        for (auto j = i->begin(); j != i->end(); ++j) {
            result.at(std::distance(i->begin(), j)).push_back(*j);
        }
    }
    return result;
}

auto calculate_expanded_rows(Input const& input) -> Numbers {
    Numbers rows;
    for (auto i = input.begin(); i != input.end(); ++i) {
        auto const is_dot = [](char c) { return c == '.'; };
        if (std::all_of(i->begin(), i->end(), is_dot)) {
            rows.push_back(std::distance(input.begin(), i));
        }
    }
    return rows;
}

auto find_galaxies(Input const& input) -> Numbers {
    Numbers positions;
    for (auto i = input.begin(); i != input.end(); ++i) {
        auto count = std::count(i->begin(), i->end(), '#');
        while (count--) {
            positions.push_back(std::distance(input.begin(), i));
        }
    }
    return positions;
}

auto expand(Numbers galaxies, Numbers const& expansions) -> Numbers {
    // Assumption: expansions and galaxies are sorted. We can start form the
    // last expansion and add 1 to only those which are further. This will cause
    // the expansions to correctly apply the correct number of times.
    for (auto i = expansions.rbegin(); i != expansions.rend(); ++i) {
        auto const& expansion = *i;
        auto const expand_once = [&expansion](Number galaxy) {
            return galaxy + (galaxy > expansion);
        };
        std::transform(
            galaxies.begin(), galaxies.end(), galaxies.begin(), expand_once);
    }
    return galaxies;
}

auto abs(Number x) -> Number { return x >= 0 ? x : -x; }

auto sum_all_distances(Numbers const& galaxies) -> Number {
    Number sum{};
    for (auto i = galaxies.begin(); i != galaxies.end(); ++i) {
        for (auto j = std::next(i); j != galaxies.end(); ++j) {
            sum += abs(*i - *j);
        }
    }
    return sum;
}

auto debug_print(Numbers const& numbers) -> void {
    std::cout << "NUMBERS";
    for (auto const& number : numbers) {
        std::cout << ' ' << number;
    }
    std::cout << '\n';
}

auto debug_print(Input const& input) -> void {
    for (auto const& line : input) {
        std::cout << line << '\n';
    }
}

auto partial_solve(Input const& input) -> Number {
    debug_print(input);
    auto const expansion_positions = calculate_expanded_rows(input);
    debug_print(expansion_positions);
    auto const galaxies_positions = find_galaxies(input);
    debug_print(galaxies_positions);
    auto const expanded = expand(galaxies_positions, expansion_positions);
    debug_print(expanded);
    auto const sum = sum_all_distances(expanded);
    return sum;
}

auto solve(Input const& input) -> Number {
    std::cout << "INPUT\n";
    auto vertical = partial_solve(input);
    std::cout << "INVERTED\n";
    auto horizontal = partial_solve(invert(input));
    return vertical + horizontal;
}

auto main() -> int {
    auto const input = parse(std::cin);
    std::cout << solve(input) << std::endl;
    return 0;
}
