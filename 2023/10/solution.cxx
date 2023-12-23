#include <algorithm>
#include <deque>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <utility>
#include <vector>

using Line = std::string;
using Input = std::vector<Line>;
using Number = int;
using Point = std::pair<Number, Number>;
using Point_opt = std::optional<Point>;
using Point_opt_pair = std::pair<Point_opt, Point_opt>;
using Points = std::deque<Point>;
using Point_pair = std::pair<Point, Point>;
using Point_pair_opt = std::optional<Point_pair>;
using Numbers = std::vector<Number>;
using Grid = std::vector<Numbers>;

auto parse(std::istream& input) -> Input {
    Input grid;
    Line line;
    while (std::getline(input, line)) {
        grid.push_back(line);
    }
    return grid;
}

auto generate_empty_grid(Input const& input) -> Grid {
    Grid grid(input.size());
    for (auto& numbers : grid) {
        numbers = Numbers(input.at(0).size());
    }
    return grid;
}

auto fits(Number begin, Number value, Number end) -> bool {
    return value >= begin && value < end;
}

auto raw_lookup(Input const& input, Point const& point) -> char {
    auto const length = input.size();
    auto const width = input.at(0).size();
    return (fits(0, point.first, length) && fits(0, point.second, width)) ?
               input.at(point.first).at(point.second) :
               '.';
}

auto step_n(Point const& p) -> Point {
    return std::make_pair(p.first - 1, p.second);
}
auto step_e(Point const& p) -> Point {
    return std::make_pair(p.first, p.second + 1);
}
auto step_s(Point const& p) -> Point {
    return std::make_pair(p.first + 1, p.second);
}
auto step_w(Point const& p) -> Point {
    return std::make_pair(p.first, p.second - 1);
}

auto goes_n(char c) -> bool { return c == '|' || c == 'L' || c == 'J'; }
auto goes_e(char c) -> bool { return c == '-' || c == 'L' || c == 'F'; }
auto goes_s(char c) -> bool { return c == '|' || c == '7' || c == 'F'; }
auto goes_w(char c) -> bool { return c == '-' || c == '7' || c == 'J'; }

auto deduce_tile(Input const& input, Point const& point) -> char {
    auto const n = raw_lookup(input, step_n(point));
    auto const e = raw_lookup(input, step_e(point));
    auto const s = raw_lookup(input, step_s(point));
    auto const w = raw_lookup(input, step_w(point));
    return (goes_s(n) && goes_n(s)) ? '|' :
           (goes_e(w) && goes_w(e)) ? '-' :
           (goes_s(n) && goes_w(e)) ? 'L' :
           (goes_s(n) && goes_e(w)) ? 'J' :
           (goes_n(s) && goes_e(w)) ? '7' :
           (goes_n(s) && goes_w(e)) ? 'F' :
                                      '.';
}

auto lookup(Input const& input, Point const& point) -> char {
    auto const value = raw_lookup(input, point);
    return value == 'S' ? deduce_tile(input, point) : value;
}

auto find_start_point(Input const& input) -> Point {
    for (auto row_iter = input.begin(); row_iter != input.end(); ++row_iter) {
        auto col_iter = std::find(row_iter->begin(), row_iter->end(), 'S');
        if (col_iter != row_iter->end()) {
            return std::make_pair(
                std::distance(input.begin(), row_iter),
                std::distance(row_iter->begin(), col_iter));
        }
    }
    return {};
}

auto operator<<(std::ostream& os, Point const& point) -> std::ostream& {
    return os << '(' << point.first << ',' << point.second << ')';
}

template <typename T>
auto debug_print(T const& grid) -> void {
    for (auto const& line : grid) {
        auto first = true;
        for (auto const& value : line) {
            if (!first) {
                // std::cout << '\t';
            }
            first = false;
            std::cout << value;
        }
        std::cout << '\n';
    }
}

auto lookup(Grid const& grid, Point const& point) -> Number {
    return grid[point.first][point.second];
}

template <typename T>
auto enter(T& grid, Point const& point, Number value) -> void {
    grid[point.first][point.second] = value;
}

auto generate_next_points_from_pipe(Input const& input, Point const& point)
    -> Point_pair_opt {
    auto const value = lookup(input, point);
    return value == '|' ? Point_pair_opt{{step_n(point), step_s(point)}} :
           value == '-' ? Point_pair_opt{{step_e(point), step_w(point)}} :
           value == 'L' ? Point_pair_opt{{step_n(point), step_e(point)}} :
           value == 'J' ? Point_pair_opt{{step_n(point), step_w(point)}} :
           value == '7' ? Point_pair_opt{{step_s(point), step_w(point)}} :
           value == 'F' ? Point_pair_opt{{step_s(point), step_e(point)}} :
                          Point_pair_opt{/* should not happen */};
}

auto generate_next_points(
    Input const& input, Grid const& grid, Point const& point)
    -> Point_opt_pair {
    // It should be always possible to get value given the task constraints.
    auto const [a, b] = generate_next_points_from_pipe(input, point).value();
    return {
        lookup(grid, a) ? Point_opt{} : Point_opt{a},
        lookup(grid, b) ? Point_opt{} : Point_opt{b}};
}

auto max(Grid const& grid) -> Number {
    return std::transform_reduce(
        grid.begin(), grid.end(), std::numeric_limits<Number>::min(),
        [](Number a, Number b) { return std::max(a, b); },
        [](Numbers const& nums) {
            return *std::max_element(nums.begin(), nums.end());
        });
}

auto trace_path(Input const& input) -> Grid {
    auto grid = generate_empty_grid(input);
    auto start = find_start_point(input);
    Points points{start};
    enter(grid, start, 1);
    while (!points.empty()) {
        auto const point = points.front();
        points.pop_front();
        auto const point_value = lookup(grid, point);
        auto const [a_opt, b_opt] = generate_next_points(input, grid, point);
        if (a_opt.has_value()) {
            points.push_back(*a_opt);
            enter(grid, *a_opt, point_value + 1);
        }
        if (b_opt.has_value()) {
            points.push_back(*b_opt);
            enter(grid, *b_opt, point_value + 1);
        }
    }
    return grid;
}

auto solve(Input const& input) -> Number {
    auto const grid = trace_path(input);
    debug_print(grid);
    return max(grid) - 1;
}

auto expand(Line const& line) -> Line {
    Line result;
    result.reserve(line.size() * 2 + 1);
    bool first = true;
    for (auto symbol : line) {
        result += first ? '.' : '-';
        first = false;
        result += symbol;
    }
    result += '.';
    return result;
}

auto make_spacer(size_t size) -> Line {
    Line line;
    line.reserve(size);
    while (size--) {
        line += size % 2 ? '|' : '.';
    }
    return line;
}

auto expand(Input const& input) -> Input {
    auto const new_width = input[0].size() * 2 + 1;
    Input result;
    result.reserve(input.size() * 2 + 1);
    result.emplace_back(new_width, '.');
    auto first = true;
    for (auto const& line : input) {
        if (!first) {
            result.push_back(make_spacer(new_width));
        }
        first = false;
        result.push_back(expand(line));
    }
    result.emplace_back(new_width, '.');
    // Make fix for S point deduction
    auto const input_s_point = find_start_point(input);
    auto const result_s_point = find_start_point(result);
    if (!goes_s(raw_lookup(input, step_n(input_s_point)))) {
        enter(result, step_n(result_s_point), '.');
    }
    if (!goes_n(raw_lookup(input, step_s(input_s_point)))) {
        enter(result, step_s(result_s_point), '.');
    }
    if (!goes_e(raw_lookup(input, step_w(input_s_point)))) {
        enter(result, step_w(result_s_point), '.');
    }
    if (!goes_w(raw_lookup(input, step_e(input_s_point)))) {
        enter(result, step_e(result_s_point), '.');
    }
    return result;
}

auto contract(Line const& line) -> Line {
    Line result;
    result.reserve((line.size() - 1) / 2);
    for (auto i = std::next(line.begin()); i != line.end(); i += 2) {
        result += *i;
    }
    return result;
}

auto contract(Input const& input) -> Input {
    Input result;
    result.reserve((input.size() - 1) / 2);
    for (auto i = input.begin(); i != input.end(); ++i) {
        if (std::distance(input.begin(), i) % 2) {
            result.push_back(contract(*i));
        }
    }
    return result;
}

auto solve2(Input const& input) -> Number {
    std::cout << "INPUT\n";
    debug_print(input);
    auto const expanded = expand(input);
    std::cout << "EXPANDED\n";
    debug_print(expanded);
    // TODO
    auto const contracted = contract(expanded);
    std::cout << "CONTRACTED\n";
    debug_print(contracted);
    return 123;
}

auto main() -> int {
    auto const input = parse(std::cin);
    debug_print(input);
    std::cout << solve(input) << std::endl;
    std::cout << solve2(input) << std::endl;
    return 0;
}
