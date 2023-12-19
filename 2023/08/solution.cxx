#include <algorithm>
#include <array>
#include <iostream>
#include <unordered_map>

struct Steps {
    Steps(std::string const& _steps) : steps{_steps} {};
    auto get_step() -> char {
        if (rotations == 0) {
            ++rotations;
            iter = steps.cbegin();
        }
        auto step = *iter;
        if (++iter == steps.cend()) {
            iter = steps.cbegin();
            ++rotations;
        }
        return step;
    }
    auto get_count() const -> unsigned int {
        return steps.size() * (rotations - 1) +
               std::distance(steps.cbegin(), iter);
    }

private:
    std::string steps;
    std::string::const_iterator iter;
    unsigned int rotations{};
};

using Id = std::array<char, 3>;
using Entry = std::pair<Id, Id>;
using Atlas = std::unordered_map<Id, Entry>;

template <>
struct std::hash<Id> {
    std::size_t operator()(const Id& id) const {
        return static_cast<int>(id[0]) << 16 ^ static_cast<int>(id[1]) << 8 ^
               static_cast<int>(id[0]);
    }
};

auto parse_id(std::string::const_iterator i) -> Id {
    return {*i, *(i + 1), *(i + 2)};
}

auto parse_atlas_line(std::string const& line) -> std::pair<Id, Entry> {
    return std::make_pair(
        parse_id(line.cbegin()),
        std::make_pair(
            parse_id(std::find(line.cbegin(), line.cend(), '(') + 1),
            parse_id(std::find(line.cbegin(), line.cend(), ',') + 2)));
}

auto parse(std::istream& input) -> std::pair<Steps, Atlas> {
    std::string steps;
    std::getline(input, steps);
    std::string line;
    std::getline(input, line);  // Skip
    Atlas atlas;
    while (std::getline(input, line)) {
        atlas.emplace(parse_atlas_line(line));
    }
    return std::make_pair(Steps{steps}, atlas);
}

auto operator<<(std::ostream& out, Id const& id) -> std::ostream& {
    return out << id[0] << id[1] << id[2];
}

auto print(Atlas const& atlas) -> void {
    for (auto const& [from, to] : atlas) {
        std::cout << "(" << from << ") -> L(" << to.first << ") R(" << to.second
                  << ")\n";
    }
}

auto main() -> int {
    auto [steps, atlas] = parse(std::cin);
    print(atlas);
    auto current = Id{'A', 'A', 'A'};
    auto const target = Id{'Z', 'Z', 'Z'};
    while (current != target) {
        current = (steps.get_step() == 'L') ? atlas[current].first :
                                              atlas[current].second;
    }
    std::cout << steps.get_count() << std::endl;
    return 0;
}
