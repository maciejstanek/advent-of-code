#include <iostream>
#include <set>

int solve(std::string const& message, int size) {
  for (auto i = message.begin(); i + size != message.end(); ++i) {
    std::set<char> chars(i, i + size);
    if (static_cast<int>(chars.size()) == size) {
      return std::distance(message.begin(), i) + size;
    }
  }
  return -1;
}

int main()
{
  std::string message;
  std::cin >> message;
  std::cout << solve(message, 4) << "\n";
  return 0;
}
