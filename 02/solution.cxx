#include <iostream>
#include <numeric>
#include <string>
#include <vector>

enum Shape { Rock, Paper, Scissors };

int shape_score(Shape shape) { return static_cast<int>(shape) + 1; }

struct Duel {
  Shape player;
  Shape oponent;
};

int duel_score(Duel duel)
{
  auto const& oponent = duel.oponent;
  auto const& player = duel.player;
  auto constexpr DEFEAT_SCORE = 0;
  auto constexpr DRAW_SCORE = 3;
  auto constexpr WIN_SCORE = 6;
  if (oponent == player) {
    return DRAW_SCORE;
  } else if ((player == Shape::Scissors && oponent == Shape::Paper)
      || (player == Shape::Paper && oponent == Shape::Rock)
      || (player == Shape::Rock && oponent == Shape::Scissors)) {
    return WIN_SCORE;
  } else {
    return DEFEAT_SCORE;
  }
}

using Strategy = std::vector<Duel>;

Shape parse_shape(char value)
{
  return (value == 'A' || value == 'X') ? Shape::Rock
      : (value == 'B' || value == 'Y')  ? Shape::Paper
                                        : Shape::Scissors;
}

Duel parse_duel(std::string const& duel)
{
  return { parse_shape(duel.at(2)), parse_shape(duel.at(0)) };
}

Strategy parse_input(std::istream& input)
{
  Strategy strategy;
  while (!input.eof()) {
    std::string line;
    std::getline(input, line);
    if (line.empty()) {
      continue;
    }
    strategy.emplace_back(parse_duel(line));
  }
  return strategy;
}

int solve(Strategy const& strategy)
{
  return std::transform_reduce(strategy.begin(), strategy.end(), 0, std::plus<>(),
      [](auto const& duel) { return duel_score(duel) + shape_score(duel.player); });
}

int main()
{
  std::cout << solve(parse_input(std::cin)) << "\n";
  return 0;
}
