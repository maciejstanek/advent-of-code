#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using Count = int;
using Direction = char;
struct Motion {
  Direction direction;
  Count count;
};
std::ostream& operator<<(std::ostream& out, Motion const& motion)
{
  return out << "[" << motion.direction << "," << motion.count << "]";
}
using Motions = std::vector<Motion>;
struct Position {
  int x; // horizontal left to right
  int y; // vertical bottom to top
  bool operator<(Position const& other) const
  {
    return x == other.x ? y < other.y : x < other.x;
  }
  Position rotated() const { return { y, x }; }
};
std::ostream& operator<<(std::ostream& out, Position const& position)
{
  return out << "{" << position.x << "," << position.y << "}";
}
using Position_memo = std::set<Position>;
std::ostream& operator<<(std::ostream& out, Position_memo const& memo)
{
  out << "{";
  auto first = true;
  for (auto const& position : memo) {
    if (!first) {
      out << ",";
    }
    out << position;
    if (first) {
      first = false;
    }
  }
  return out << "}";
}
struct Rope {
  Position head;
  Position tail;
};

std::ostream& operator<<(std::ostream& out, Rope const& rope)
{
  return out << "(H" << rope.head << ")--<--(T" << rope.tail << ")";
}

Motions parse(std::istream& input)
{
  Motions motions;
  while (!input.eof()) {
    std::string line;
    std::getline(input, line);
    if (line.empty()) {
      break;
    }
    motions.push_back({ line.at(0),
        std::stoi(std::string { line.begin() + 2, line.end() }) });
  }
  return motions;
}

struct Motion_result {
  Position_memo memo;
  Rope rope;
};

Position move_head(Position const& position, Direction direction)
{
  return direction == 'U' ? Position { position.x, position.y + 1 }
      : direction == 'D'  ? Position { position.x, position.y - 1 }
      : direction == 'L'  ? Position { position.x - 1, position.y }
                          : Position { position.x + 1, position.y };
}

Position move_tail(Position const& tail, Position const& head)
{
  return (std::clamp(tail.x, head.x - 1, head.x + 1) == tail.x
             && std::clamp(tail.y, head.y - 1, head.y + 1) == tail.y)
      ? tail
      : (head.x == tail.x)
      ? Position { tail.x, std::clamp(tail.y, head.y - 1, head.y + 1) }
      : (head.y == tail.y)
      ? Position { std::clamp(tail.x, head.x - 1, head.x + 1), tail.y }
      : (head.x > tail.x && head.y > tail.y)
      ? Position { tail.x + 1, tail.y + 1 }
      : (head.x > tail.x && head.y < tail.y)
      ? Position { tail.x + 1, tail.y - 1 }
      : (head.x < tail.x && head.y > tail.y)
      ? Position { tail.x - 1, tail.y + 1 }
      : (head.x < tail.x && head.y < tail.y)
      ? Position { tail.x - 1, tail.y - 1 }
      : tail;
}

Rope move_rope(Rope rope, Direction direction)
{
  rope.head = move_head(rope.head, direction);
  rope.tail = move_tail(rope.tail, rope.head);
  return rope;
}

Motion_result move_rope(Rope rope, Motion const& motion)
{
  Position_memo memo;
  for (auto i = 0; i < motion.count; ++i) {
    rope = move_rope(rope, motion.direction);
    memo.insert(rope.tail);
  }
  return { memo, rope };
}

int solve(Motions const& motions)
{
  Position_memo memo;
  Rope rope {};
  memo.insert(rope.tail);
  for (auto const& motion : motions) {
    auto result = move_rope(rope, motion);
    memo.insert(result.memo.begin(), result.memo.end());
    rope = result.rope;
    std::cerr << "MOTION = " << motion << "\n";
    std::cerr << "ROPE STATE = " << rope << "\n";
    std::cerr << "PARTIAL MEMO = " << result.memo << "\n";
  }
  return memo.size();
}

int main()
{
  auto commands = parse(std::cin);
  std::cout << solve(commands) << "\n";
  return 0;
}
