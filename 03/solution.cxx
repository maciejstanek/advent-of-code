#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using Strings = std::vector<std::string>;

Strings parse_input(std::istream& input)
{
  Strings lines;
  while (!input.eof()) {
    std::string line;
    std::getline(input, line);
    if (line.empty()) {
      continue;
    }
    lines.emplace_back(line);
  }
  return lines;
}

char solve(std::string const& line) {
  std::set<char> left;
  std::set<char> right;
  auto half = line.begin() + line.size() / 2;
  std::copy(line.begin(), half, std::inserter(left, left.end()));
  std::copy(half, line.end(), std::inserter(right, right.end()));
  std::vector<char> intersection;
  std::set_intersection(
      left.begin(), left.end(), right.begin(), right.end(), std::back_inserter(intersection));
  return intersection.front();
}

constexpr int priority(char item) {
  return (item < 'a') ? item - 'A' + 27 : item - 'a' + 1;
}

int solve(Strings const& strings)
{
  return std::transform_reduce(strings.begin(), strings.end(), 0, std::plus<>(),
      [](auto const& line) { return priority(solve(line)); });
}

int main()
{
  std::cout << solve(parse_input(std::cin)) << "\n";
  return 0;
}
