#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <numeric>

struct Command {
  virtual ~Command() = default;
  virtual void print(std::ostream&) const = 0;
};

struct Enter : Command {
  std::string target;
  Enter(auto const& target_)
      : target { target_ }
  {
  }
  void print(std::ostream& output) const final
  {
    output << "  ENTER " << target << "\n";
  }
};

struct Leave : Command {
  void print(std::ostream& output) const final { output << "  LEAVE\n"; }
};

struct List : Command {
  // Encoding DIR on 0.
  std::map<std::string, int> results;
  List(auto const& results_)
      : results { results_ }
  {
  }
  void print(std::ostream& output) const final
  {
    output << "  LIST";
    for (auto const& [name, size] : results) {
      output << " " << name << "(" << size << ")";
    }
    output << "\n";
  }
};

std::vector<std::string> get_lines(std::istream& input)
{
  std::vector<std::string> lines;
  while (!input.eof()) {
    std::string line;
    std::getline(input, line);
    if (line.empty()) {
      break;
    }
    lines.push_back(line);
  }
  return lines;
}

using Commands = std::vector<std::unique_ptr<Command>>;

Commands parse(std::istream& input)
{
  auto lines = get_lines(input);
  Commands commands;
  // +1 to skip "cd /" which is always at the beginning.
  for (auto lineIter = lines.begin() + 1; lineIter != lines.end(); ++lineIter) {
    assert(lineIter->at(0) == '$');
    if (*lineIter == "$ cd ..") {
      commands.emplace_back(std::make_unique<Leave>());
    } else if (lineIter->at(2) == 'c') {
      auto target = std::string(lineIter->begin() + 5, lineIter->end());
      commands.emplace_back(std::make_unique<Enter>(target));
    } else if (lineIter->at(2) == 'l') {
      std::map<std::string, int> results;
      while (std::next(lineIter) != lines.end()
          && std::next(lineIter)->at(0) != '$') {
        lineIter += 1;
        auto space = std::find(lineIter->begin(), lineIter->end(), ' ');
        auto sizeString = std::string { lineIter->begin(), space };
        auto size = sizeString == "dir" ? 0 : std::stoi(sizeString);
        auto name = std::string { space + 1, lineIter->end() };
        results[name] = size;
      }
      commands.emplace_back(std::make_unique<List>(results));
    }
  }
  return commands;
}

void print_commands(std::ostream& output, Commands const& commands)
{
  output << "[COMMANDS BEGIN]\n";
  for (auto const& command : commands) {
    command->print(output);
  }
  output << "[COMMANDS END]\n";
}

struct Tree {
  Tree* parent = nullptr;
  std::vector<Tree> leaves;
  std::string name;
  int size = 0;
  Tree(Tree* parent_, std::string const& name_, int size_)
      : parent { parent_ }
      , name { name_ }
      , size { size_ }
  {
  }
  Tree* leaf(std::string const& name)
  {
    auto node = std::find_if(leaves.begin(), leaves.end(),
        [&](auto const& node) { return node.name == name; });
    assert(node != leaves.end());
    return &*node;
  }
};

std::ostream& operator<<(std::ostream& output, Tree const& tree)
{
  output << "(" << tree.name << "~" << tree.size;
  for (auto const& leaf : tree.leaves) {
    output << " " << leaf;
  }
  return output << ")";
}

int fill_sizes(Tree& tree)
{
  if (tree.leaves.empty()) {
    return tree.size;
  }
  return tree.size
      = std::transform_reduce(tree.leaves.begin(), tree.leaves.end(), 0,
          std::plus<int> {}, [](auto& tree) { return fill_sizes(tree); });
}

std::unique_ptr<Tree> make_graph(Commands const& commands)
{
  auto root = std::make_unique<Tree>(nullptr, "/", 0);
  auto node = root.get();
  for (auto const& command : commands) {
    if (auto leave = dynamic_cast<Leave*>(command.get())) {
      node = node->parent;
    } else if (auto enter = dynamic_cast<Enter*>(command.get())) {
      node = node->leaf(enter->target);
    } else if (auto listc = dynamic_cast<List*>(command.get())) {
      for (const auto& [name, size] : listc->results) {
        node->leaves.emplace_back(node, name, size);
      }
    }
  }
  fill_sizes(*root.get());
  return root;
}

void solve_impl(Tree const& tree, int& sum) {
  if (!tree.leaves.empty()) {
    if (tree.size <= 100000) {
      sum += tree.size;
    }
  }
  for (auto const& node : tree.leaves) {
    solve_impl(node, sum);
  }
}

int solve(Tree const& tree) {
  auto sum = 0;
  solve_impl(tree, sum);
  return sum;
}

int main()
{
  auto commands = parse(std::cin);
  print_commands(std::cout, commands);
  auto tree = make_graph(commands);
  std::cout << *tree.get() << "\n";
  std::cout << solve(*tree.get()) << "\n";
  return 0;
}
