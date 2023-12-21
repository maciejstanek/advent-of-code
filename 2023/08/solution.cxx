#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <vector>

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
    auto size() const -> unsigned int { return steps.size(); }

private:
    std::string steps;
    std::string::const_iterator iter;
    unsigned int rotations{};
};

using Id = std::array<char, 3>;
using Ids = std::vector<Id>;
using Entry = std::pair<Id, Id>;
using Atlas = std::map<Id, Entry>;

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

auto is_finish_pos(Id const& id) -> bool {
    return id[0] == 'Z' && id[1] == 'Z' && id[2] == 'Z';
}

auto is_finish_pos_pt2(Id const& id) -> bool { return id[2] == 'Z'; }

auto is_starting_pos(Id const& id) -> bool {
    return id[0] == 'A' && id[1] == 'A' && id[2] == 'A';
}

auto is_starting_pos_pt2(Id const& id) -> bool { return id[2] == 'A'; }

typedef bool (*id_filter_t)(Id const& id);

auto generate_starting_positions(Atlas const& atlas, id_filter_t filter)
    -> Ids {
    Ids ids;
    for (auto const& [from, _] : atlas) {
        if (filter(from)) {
            ids.push_back(from);
        }
    }
    return ids;
}

auto make_step(Ids& ids, Atlas const& atlas, char dir) -> void {
    for (auto& id : ids) {
        id = (dir == 'L') ? atlas.at(id).first : atlas.at(id).second;
    }
}

auto calculate_steps(
    Atlas const& atlas, Steps steps, id_filter_t is_starting,
    id_filter_t is_finish) -> int {
    auto current = generate_starting_positions(atlas, is_starting);
    while (!std::all_of(current.begin(), current.end(), is_finish)) {
        make_step(current, atlas, steps.get_step());
    }
    return steps.get_count();
}

auto main() -> int {
    auto [steps, atlas] = parse(std::cin);
    /* print(atlas); */
    std::cout << calculate_steps(atlas, steps, is_starting_pos, is_finish_pos)
              << std::endl;
    std::cout << calculate_steps(
                     atlas, steps, is_starting_pos_pt2, is_finish_pos_pt2)
              << std::endl;
    return 0;
}
