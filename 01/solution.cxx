#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using Value = int;
using Lists = std::vector<std::vector<Value>>;

Lists parse_input(std::istream& input)
{
  Lists lists;
  lists.emplace_back();
  while (!input.eof()) {
    std::string line;
    std::getline(input, line);
    if (line.empty()) {
      lists.emplace_back();
      continue;
    }
    std::istringstream parser(line);
    Value value;
    parser >> value;
    lists.back().push_back(value);
  }
  return lists;
}

Value solve(Lists const& lists)
{
  return std::transform_reduce(
      lists.begin(), lists.end(), 0,
      [](auto previous, auto current) { return std::max(previous, current); },
      [](auto const& sublist) { return std::accumulate(sublist.cbegin(), sublist.cend(), 0); });
}

int main()
{
  std::cout << solve(parse_input(std::cin)) << "\n";
  return 0;
}
