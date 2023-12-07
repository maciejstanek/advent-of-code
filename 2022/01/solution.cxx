#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

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

Value solve(Lists const& lists, int top_count)
{
  std::vector<int> elf_sums;
  std::transform(lists.begin(), lists.end(), std::back_inserter(elf_sums),
      [](auto const& sublist) {
        return std::accumulate(sublist.cbegin(), sublist.cend(), 0);
      });
  std::sort(elf_sums.rbegin(), elf_sums.rend());
  return std::accumulate(elf_sums.begin(), elf_sums.begin() + top_count, 0);
}

int main()
{
  auto input = parse_input(std::cin);
  std::cout << solve(input, 1) << "\n";
  std::cout << solve(input, 3) << "\n";
  return 0;
}
