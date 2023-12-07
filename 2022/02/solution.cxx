#include <iostream>
#include <numeric>
#include <string>
#include <vector>

enum class Shape { Rock, Paper, Scissors };

Shape winning_shape(Shape shape)
{
  return shape == Shape::Rock ? Shape::Paper
      : shape == Shape::Paper ? Shape::Scissors
                              : Shape::Rock;
}

Shape loosing_shape(Shape shape)
{
  return shape == Shape::Rock ? Shape::Scissors
      : shape == Shape::Paper ? Shape::Rock
                              : Shape::Paper;
}

int shape_score(Shape shape) { return static_cast<int>(shape) + 1; }

struct Duel {
  Shape player;
  Shape oponent;
  struct Result {
    enum Value { Win, Draw, Defeat };
    Result() = default;
    constexpr Result(Value value_)
        : value { value_ }
    {
    }
    constexpr operator Value() const { return value; }
    constexpr operator int() const { return value == Win ? 6 : value == Draw ? 3 : 0; }
    explicit operator bool() const = delete;
    Value value;
  };
  Result expected_result() const
  {
    return player == Shape::Rock ? Result::Defeat
        : player == Shape::Paper ? Result::Draw
                                  : Result::Win;
  }
  Shape expected_player_move() const
  {
    auto const result = expected_result();
    return result == Result::Win ? winning_shape(oponent)
        : result == Result::Draw ? oponent
                                 : loosing_shape(oponent);
  }
  Result score() const
  {
    if (oponent == player) {
      return Result::Draw;
    } else if ((player == Shape::Scissors && oponent == Shape::Paper)
        || (player == Shape::Paper && oponent == Shape::Rock)
        || (player == Shape::Rock && oponent == Shape::Scissors)) {
      return Result::Win;
    } else {
      return Result::Defeat;
    }
  }
};

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
      [](auto const& duel) { return static_cast<int>(duel.score()) + shape_score(duel.player); });
}

int solve_part_2(Strategy const& strategy)
{
  return std::transform_reduce(
      strategy.begin(), strategy.end(), 0, std::plus<>(), [](auto const& duel) {
        return static_cast<int>(duel.expected_result()) + shape_score(duel.expected_player_move());
      });
}

int main()
{
  auto input = parse_input(std::cin);
  std::cout << "part 1: " << solve(input) << "\n";
  std::cout << "part 2: " << solve_part_2(input) << "\n";
  return 0;
}
