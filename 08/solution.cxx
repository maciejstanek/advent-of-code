#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

using Row = std::vector<int>;
using Forest = std::vector<Row>;

Forest parse(std::istream& input)
{
  Forest rows;
  while (!input.eof()) {
    std::string line;
    std::getline(input, line);
    if (line.empty()) {
      break;
    }
    Row row;
    std::transform(line.cbegin(), line.cend(), std::back_inserter(row),
        [](auto tree) { return tree - '0'; });
    rows.push_back(row);
  }
  return rows;
}

std::ostream& operator<<(std::ostream& output, Row const& row)
{
  std::transform(row.cbegin(), row.cend(), std::ostream_iterator<char>(output),
      [](auto value) -> char { return value + '0'; });
  return output;
}

std::ostream& operator<<(std::ostream& output, Forest const& forest)
{
  output << "[FOREST BEGIN]\n";
  for (auto row : forest) {
    std::transform(row.cbegin(), row.cend(), row.begin(),
        [](auto tree) { return tree + '0'; });
    output << row << "\n";
  }
  return output << "[FOREST END]\n";
}

Forest mirror(Forest const& forest)
{
  Forest result;
  std::transform(forest.begin(), forest.end(), std::back_inserter(result),
      [](auto const& row) -> Row {
        return { row.crbegin(), row.crend() };
      });
  return result;
}

Forest flip(Forest forest)
{
  for (auto row = 0u; row != forest.size(); ++row) {
    for (auto tree = row + 1; tree != forest[row].size(); ++tree) {
      std::swap(forest[row][tree], forest[tree][row]);
    }
  }
  return forest;
}

using Rotations = std::array<Forest, 4>;

Rotations make_rotations(Forest const& forest)
{
  return { forest, flip(forest), mirror(forest), mirror(flip(forest)) };
}

Rotations undo_rotations(Rotations const& rotations)
{
  return { rotations[0], flip(rotations[1]), mirror(rotations[2]),
    flip(mirror(rotations[3])) };
}

Row mark_trees_from_left(Row row)
{
  std::inclusive_scan(
      row.cbegin(), row.cend(), row.begin(),
      [](char left, char right) { return std::max(left, right); }, 0);
  std::adjacent_difference(row.cbegin(), row.cend(), row.begin(),
      [](auto right, auto left) { return std::clamp(right - left, 0, 1); });
  row.at(0) = 1; // Can always see the first tree.
  return row;
}

Forest mark_trees_from_left(Forest forest)
{
  std::transform(forest.cbegin(), forest.cend(), forest.begin(),
      [](auto const& row) { return mark_trees_from_left(row); });
  return forest;
}

Rotations mark_trees_from_left(Rotations rotations)
{
  std::transform(rotations.cbegin(), rotations.cend(), rotations.begin(),
      [](auto const& forest) { return mark_trees_from_left(forest); });
  return rotations;
}

template <typename Op>
Row combine_masks(Row mask1, Row const& mask2, Op const& op)
{
  std::transform(
      mask1.cbegin(), mask1.cend(), mask2.cbegin(), mask1.begin(), op);
  return mask1;
}

template <typename Op>
Forest combine_masks(Forest mask1, Forest const& mask2, Op const& op)
{
  std::transform(mask1.cbegin(), mask1.cend(), mask2.cbegin(), mask1.begin(),
      [&op](auto left, auto right) { return combine_masks(left, right, op); });
  return mask1;
}

template <typename Op>
Forest combine_rotations(Rotations rotations, Op const& op)
{
  auto masks = undo_rotations(rotations);
  return std::reduce(std::next(masks.cbegin()), masks.cend(), masks[0],
      [&op](auto const& left, auto const& right) {
        return combine_masks(left, right, op);
      });
}

int solve(Forest forest)
{
  auto mask = combine_rotations(
      mark_trees_from_left(make_rotations(forest)), std::logical_or {});
  return std::transform_reduce(
      mask.cbegin(), mask.cend(), 0, std::plus {}, [](auto const& row) {
        return std::accumulate(row.cbegin(), row.cend(), 0);
      });
}

Row calculate_view(Row row)
{
  for (auto i = row.begin(); i != row.end(); ++i) {
    auto j = std::find_if(
        std::next(i), row.end(), [&i](auto tree) { return tree >= *i; });
    *i = std::distance(i, j) + (j == row.end() ? -1 : 0);
  }
  return row;
}

Forest calculate_view(Forest forest)
{
  std::transform(forest.cbegin(), forest.cend(), forest.begin(),
      [](auto const& row) { return calculate_view(row); });
  return forest;
}

Rotations calculate_view(Rotations rotations)
{
  std::transform(rotations.cbegin(), rotations.cend(), rotations.begin(),
      [](auto const& forest) { return calculate_view(forest); });
  return rotations;
}

int solve_view(Forest forest)
{
  constexpr const auto max = [](int a, int b) { return std::max(a, b); };
  auto mask = combine_rotations(
      calculate_view(make_rotations(forest)), std::multiplies {});
  return std::transform_reduce(
      mask.cbegin(), mask.cend(), 0, max, [&max](auto const& row) {
        return std::reduce(row.cbegin(), row.cend(), 0, max);
      });
}

int main()
{
  auto forest = parse(std::cin);
  std::cout << forest << "\n";
  std::cout << "cover " << solve(forest) << "\n";
  std::cout << "right " << calculate_view(forest) << "\n";
  std::cout << "left " << mirror(calculate_view(mirror(forest))) << "\n";
  std::cout << "down " << flip(calculate_view(flip(forest))) << "\n";
  std::cout << "up " << flip(mirror(calculate_view(mirror(flip(forest)))))
            << "\n";
  std::cout << "combined "
            << combine_rotations(
                   calculate_view(make_rotations(forest)), std::multiplies {});
  std::cout << "scenic score " << solve_view(forest) << "\n";
  return 0;
}
