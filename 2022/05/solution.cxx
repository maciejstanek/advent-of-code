#include <algorithm>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <cctype>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Command {
  int from;
  int to;
  int count;
};
using Commands = std::vector<Command>;

using Stack = std::vector<char>;
using Stacks = std::vector<Stack>;

struct Specification {
  Stacks stacks;
  Commands commands;
};

Stacks parse_raw_stacks(std::istream& input)
{
  Stacks rawStacks;
  while (!input.eof()) { // Incorrect, but for AoC will do.
    std::string line;
    std::getline(input, line);
    if (line.empty()) {
      // Completed reading starting conditions.
      break;
    }
    rawStacks.emplace_back();
    for (auto i = line.begin() + 1; i < line.end(); i += 4) {
      rawStacks.back().push_back(*i);
    }
  }
  rawStacks.pop_back(); // Just the column indexing, from 1 to N.
  return rawStacks;
}

Stacks process_raw_stacks(Stacks const& rawStacks)
{
  Stacks stacks(rawStacks.back().size());
  for (auto const& rawStack : rawStacks | boost::adaptors::reversed) {
    for (auto const& entry : rawStack | boost::adaptors::indexed(0)) {
      if (entry.value() != ' ') {
        stacks.at(entry.index()).push_back(entry.value());
      }
    }
  }
  return stacks;
}

Commands parse_commands(std::istream& input)
{
  Commands commands;
  while (!input.eof()) {
    std::string line;
    std::getline(input, line);
    if (line.empty()) {
      // Real EOF.
      break;
    }
    std::transform(line.begin(), line.end(), line.begin(),
        [](auto c) { return std::isalpha(c) ? ' ' : c; });
    Command command;
    std::stringstream temp(line);
    temp >> command.count >> command.from >> command.to;
    command.to -= 1;
    command.from -= 1;
    commands.emplace_back(command);
  }
  return commands;
}

Specification parse_input(std::istream& input)
{
  return { process_raw_stacks(parse_raw_stacks(input)), parse_commands(input) };
}

std::ostream& operator<<(
    std::ostream& output, Specification const& specification)
{
  output << "[STACKS]\n";
  for (auto const& stack : specification.stacks | boost::adaptors::indexed(0)) {
    output << "[" << stack.index() << "]:";
    for (auto value : stack.value()) {
      output << " " << value;
    }
    output << "\n";
  }
  output << "[COMMANDS]\n";
  for (auto const& command : specification.commands) {
    output << command.from << " -> " << command.to << " x" << command.count
           << "\n";
  }
  output << "[END]\n";
  return output;
}

std::string top(Stacks const& stacks) {
  std::string result;
  result.reserve(stacks.size());
  std::transform(stacks.cbegin(), stacks.cend(), std::back_inserter(result),
      [](auto const& stack) { return stack.empty() ? ' ' : stack.back(); });
  return result;
}

void solve_command(Command const& command, Stacks& stacks) {
  for (auto i = 0; i < command.count; ++i) {
    auto& from = stacks.at(command.from);
    stacks.at(command.to).push_back(from.back());
    from.pop_back();
  }
}

void solve_9001_command(Command const& command, Stacks& stacks) {
  auto& from = stacks.at(command.from);
  auto& to = stacks.at(command.to);
  std::copy(from.end() - command.count, from.end(), std::back_inserter(to));
  from.resize(from.size() - command.count);
}

std::string solve(Specification const& specification) {
  auto stacks = specification.stacks;
  for (auto const& command : specification.commands) {
    solve_command(command, stacks);
  }
  return top(stacks);
}

std::string solve_9001(Specification const& specification) {
  auto stacks = specification.stacks;
  for (auto const& command : specification.commands) {
    solve_9001_command(command, stacks);
  }
  return top(stacks);
}

int main()
{
  auto input = parse_input(std::cin);
  std::cerr << input << "\n";
  std::cout << "CrateMover 9000: " << solve(input) << "\n";
  std::cout << "CrateMover 9001: " << solve_9001(input) << "\n";
  return 0;
}
