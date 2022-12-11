#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using Row = std::string;
using Forest = std::vector<Row>;

Forest parse(std::istream& input)
{
  Forest rows;
  while (!input.eof()) {
    Row row;
    std::getline(input, row);
    if (row.empty()) {
      break;
    }
    std::transform(row.cbegin(), row.cend(), row.begin(),
        [](auto tree) { return tree - '0'; });
    rows.push_back(row);
  }
  return rows;
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

Row combine_masks(Row mask1, Row const& mask2)
{
  std::transform(mask1.cbegin(), mask1.cend(), mask2.cbegin(), mask1.begin(),
      std::logical_or {});
  return mask1;
}

Forest combine_masks(Forest mask1, Forest const& mask2)
{
  std::transform(mask1.cbegin(), mask1.cend(), mask2.cbegin(), mask1.begin(),
      [](auto left, auto right) { return combine_masks(left, right); });
  return mask1;
}

Forest combine_rotations(Rotations rotations)
{
  auto masks = undo_rotations(rotations);
  return std::reduce(std::next(masks.cbegin()), masks.cend(), masks[0],
      [](auto const& left, auto const& right) {
        return combine_masks(left, right);
      });
}

int solve(Forest forest)
{
  auto mask = combine_rotations(mark_trees_from_left(make_rotations(forest)));
  return std::transform_reduce(mask.cbegin(), mask.cend(), 0, std::plus{},
      [](auto const& row) {
        return std::accumulate(row.cbegin(), row.cend(), 0);
      });
}

int main()
{
  auto forest = parse(std::cin);
  std::cout << forest << "\n";
  std::cout << solve(forest) << "\n";
  return 0;
}
