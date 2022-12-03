#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

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

std::string common_items(std::string const& rawA, std::string const& rawB)
{
  std::set<char> a(rawA.begin(), rawA.end());
  std::set<char> b(rawB.begin(), rawB.end());
  std::string result;
  std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result));
  return result;
}

char solve(std::string const& line)
{
  auto half = line.begin() + line.size() / 2;
  std::string left(line.begin(), half);
  std::string right(half, line.end());
  return common_items(left, right).at(0);
}

constexpr int priority(char item) { return (item < 'a') ? item - 'A' + 27 : item - 'a' + 1; }

int solve2(std::string const& a, std::string const& b, std::string const& c)
{
  return priority(common_items(common_items(a, b), c).at(0));
}

int solve(Strings const& strings)
{
  return std::transform_reduce(strings.begin(), strings.end(), 0, std::plus<>(),
      [](auto const& line) { return priority(solve(line)); });
}

int solve2(Strings const& strings)
{
  // This would be cleaner if I used ranges-v3 or had c++23 as I would use the chunk algorithm.
  auto sum = 0;
  for (auto i = strings.begin(); i < strings.end(); i += 3) {
    sum += solve2(*i, *(i + 1), *(i + 2));
  }
  return sum;
}

int main()
{
  auto input = parse_input(std::cin);
  std::cout << solve(input) << "\n";
  std::cout << solve2(input) << "\n";
  return 0;
}
