#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using Line = std::string;
using Input = std::vector<Line>;
using Number = unsigned long long int;
using Numbers = std::vector<Number>;

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

auto find_galaxies_inverted(Input const& input) -> Numbers {
    Numbers positions;
    for (auto i = input[0].begin(); i != input[0].end(); ++i) {
        const auto i_pos = std::distance(input[0].begin(), i);
        for (auto j = input.begin(); j != input.end(); ++j) {
            if (j->at(i_pos) == '#') {
                positions.push_back(std::distance(input.begin(), j));
            }
        }
    }
    return positions;
}

auto expand(Numbers galaxies, Numbers const& expansions, Number factor)
    -> Numbers {
    // Assumption: expansion points are sorted. We can start from the last
    // expansion and add the factor to only those galaxies which are further
    // away. This will cause the expansions to correctly apply the desired
    // number of times.
    for (auto i = expansions.rbegin(); i != expansions.rend(); ++i) {
        auto const& expansion = *i;
        auto const expand_once = [&expansion, &factor](Number galaxy) {
            return galaxy + (galaxy > expansion ? factor - 1 : 0ull);
        };
        std::transform(
            galaxies.begin(), galaxies.end(), galaxies.begin(), expand_once);
    }
    return galaxies;
}

auto absdiff(Number x, Number y) -> Number { return x > y ? x - y : y - x; }

auto sum_all_distances(Numbers const& galaxies) -> Number {
    Number sum{};
    for (auto i = galaxies.begin(); i != galaxies.end(); ++i) {
        for (auto j = std::next(i); j != galaxies.end(); ++j) {
            sum += absdiff(*i, *j);
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

auto partial_solve(Input const& input, bool invert, Number factor) -> Numbers {
    auto const inverted = invert ? ::invert(input) : input;
    /* debug_print(inverted); */
    auto const expansion_positions = calculate_expanded_rows(inverted);
    debug_print(expansion_positions);
    auto const galaxies_positions =
        invert ? find_galaxies_inverted(inverted) : find_galaxies(inverted);
    debug_print(galaxies_positions);
    auto const expanded =
        expand(galaxies_positions, expansion_positions, factor);
    debug_print(expanded);
    return expanded;
}

auto solve(Input const& input, Number factor) -> Number {
    std::cout << "INPUT\n";
    auto const vertical = partial_solve(input, false, factor);
    std::cout << "INVERTED\n";
    auto const horizontal = partial_solve(input, true, factor);
    return sum_all_distances(vertical) + sum_all_distances(horizontal);
}

auto main() -> int {
    auto const input = parse(std::cin);
    std::cout << solve(input, 2) << std::endl;
    std::cout << solve(input, 1'000'000) << std::endl;
    return 0;
}
