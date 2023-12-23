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
using Number = signed int;
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
    return (point.first >= 0 &&
            point.first < static_cast<Number>(grid.size()) &&
            point.second >= 0 &&
            point.second < static_cast<Number>(grid[0].size())) ?
               grid[point.first][point.second] :
               std::numeric_limits<Number>::max();
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

auto contract(Numbers const& numbers) -> Numbers {
    Numbers result;
    result.reserve((numbers.size() - 1) / 2);
    for (auto i = std::next(numbers.begin()); i != numbers.end(); i += 2) {
        result.push_back(*i);
    }
    return result;
}

auto contract(Grid const& grid) -> Grid {
    Grid result;
    result.reserve((grid.size() - 1) / 2);
    for (auto i = grid.begin(); i != grid.end(); ++i) {
        if (std::distance(grid.begin(), i) % 2) {
            result.push_back(contract(*i));
        }
    }
    return result;
}

auto clamp01(Grid const& grid) -> Grid {
    Grid result(grid);
    for (auto& line : result) {
        for (auto& value : line) {
            value = value ? 1 : 0;
        }
    }
    return result;
}

auto propagate(Grid& grid, Number a, Number b) -> void {
    auto changes = 1;
    while (changes) {
        changes = 0;
        for (auto i = grid.begin(); i != grid.end(); ++i) {
            for (auto j = i->begin(); j != i->end(); ++j) {
                if (*j == a || *j == b) {
                    auto const p = Point{
                        std::distance(grid.begin(), i),
                        std::distance(i->begin(), j)};
                    if (auto n = step_n(p); !lookup(grid, n)) {
                        enter(grid, n, *j);
                        ++changes;
                    }
                    if (auto e = step_e(p); !lookup(grid, e)) {
                        enter(grid, e, *j);
                        ++changes;
                    }
                    if (auto s = step_s(p); !lookup(grid, s)) {
                        enter(grid, s, *j);
                        ++changes;
                    }
                    if (auto w = step_w(p); !lookup(grid, w)) {
                        enter(grid, w, *j);
                        ++changes;
                    }
                }
            }
        }
    }
}

auto count(Grid const& grid, Number neddle) -> Number {
    return std::transform_reduce(
        grid.begin(), grid.end(), 0, std::plus{},
        [&neddle](Numbers const& numbers) {
            return std::count(numbers.begin(), numbers.end(), neddle);
        });
}

auto print_with_colors(Grid const& grid) -> void {
    for (const auto& line : grid) {
        for (auto v : line) {
            std::cout << "\033[0;"
                      << (v == 0 ? 30 :
                          v == 1 ? 37 :
                          v == 2 ? 31 :
                                   32)
                      << "m"
                      << (v == 0 ? '.' :
                          v == 1 ? '#' :
                                   '+')
                      << "\033[0m";
        }
        std::cout << "\n";
    }
}

auto solve2(Input const& input) -> Number {
    auto const expanded = expand(input);
    auto grid = clamp01(trace_path(expanded));
    auto const start = find_start_point(expanded);
    auto const n = lookup(grid, step_n(start));
    auto const e = lookup(grid, step_e(start));
    auto const s = lookup(grid, step_s(start));
    auto const w = lookup(grid, step_w(start));
    auto const red = 2;
    auto const blue = 3;
    if (n && s) {
        enter(grid, step_e(start), red);
        enter(grid, step_w(start), blue);
    } else if (e && w) {
        enter(grid, step_n(start), red);
        enter(grid, step_s(start), blue);
    } else if (n && e) {
        enter(grid, step_n(step_e(start)), red);
        enter(grid, step_s(start), blue);
    } else if (e && s) {
        enter(grid, step_e(step_s(start)), red);
        enter(grid, step_w(start), blue);
    } else if (s && w) {
        enter(grid, step_s(step_w(start)), red);
        enter(grid, step_n(start), blue);
    } else if (w && n) {
        enter(grid, step_w(step_n(start)), red);
        enter(grid, step_e(start), blue);
    }
    propagate(grid, red, blue);
    print_with_colors(grid);
    auto const contracted = contract(grid);
    print_with_colors(contracted);
    auto neddle = grid[0][0] == red ? blue : red;
    return count(contracted, neddle);
}

auto main() -> int {
    auto const input = parse(std::cin);
    debug_print(input);
    std::cout << solve(input) << std::endl;
    std::cout << solve2(input) << std::endl;
    return 0;
}
