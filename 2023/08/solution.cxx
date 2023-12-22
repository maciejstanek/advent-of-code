#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
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
    auto get_count() const -> unsigned long {
        return steps.size() * (rotations - 1ul) +
               std::distance(steps.cbegin(), iter);
    }

private:
    std::string steps;
    std::string::const_iterator iter;
    unsigned long rotations{};
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

auto is_finish_pos(Id const& id) -> bool {
    return id[0] == 'Z' && id[1] == 'Z' && id[2] == 'Z';
}

auto is_finish_pos_pt2(Id const& id) -> bool { return id[2] == 'Z'; }

auto is_starting_pos(Id const& id) -> bool {
    return id[0] == 'A' && id[1] == 'A' && id[2] == 'A';
}

auto is_starting_pos_pt2(Id const& id) -> bool { return id[2] == 'A'; }

auto print(Atlas const& atlas) -> void {
    std::cout << "digraph aoc2023_d8 {\n";
    std::cout << "  graph [splines=true overlap=false];\n";
    std::set<Id> nodes;
    auto const print_node = [](Id const& id) {
        std::cout << "  \"" << id << "\"";
        if (is_starting_pos_pt2(id)) {
            std::cout << " [style=filled, fillcolor=red]";
        }
        if (is_finish_pos_pt2(id)) {
            std::cout << " [style=filled, fillcolor=green]";
        }
        std::cout << ";\n";
    };
    for (auto const& [from, to] : atlas) {
        auto const& [left, right] = to;
        if (!nodes.count(from)) {
            nodes.insert(from);
            print_node(from);
        }
        if (!nodes.count(left)) {
            nodes.insert(left);
            print_node(left);
        }
        if (!nodes.count(right)) {
            nodes.insert(right);
            print_node(right);
        }
        std::cout << "  \"" << from << "\" -> \"" << left << "\"; \"" << from
                  << "\" -> \"" << right << "\";\n";
    }
    std::cout << "}\n";

    // Graph analysis for part 2
    //  * The graph is divided into disjoint loops
    //  * Each loop has precisely one start and one finish
    //  * Both start and finish lead to the same nodes after 1 step
    //  * Each start should be considered an independent case
    //  * The loop consists of pairs of nodes (aside from the start)
    //  * Each pair leads to the next pair, regardless of the direction
    //  * Therefore, we will hit finish every single time we circle back
    //  * Therefore, all loops will land on the end nodes when they perform the
    //  LCM number of full loops
    //  * To do: 1) calculate #steps to get from start to finish, 2) calculate
    //  LCM
}

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

auto generate_primes(unsigned long n) -> std::set<unsigned long> {
    // Algorithm with no optimizations and no memoization
    std::vector<unsigned long> sieve(n);
    std::iota(sieve.begin(), sieve.end(), 0);
    sieve[1] = 0ul;
    for (auto i = 2ul; i < n; ++i) {
        if (sieve[i] != 0ul) {
            for (auto j = 2ul * i; j < n; j += i) {
                sieve[j] = 0ul;
            }
        }
    }
    std::set<unsigned long> results;
    std::copy_if(
        sieve.begin(), sieve.end(), std::inserter(results, results.begin()),
        [](unsigned long x) -> bool { return x; });
    return results;
}

auto prime_factors(unsigned long x) -> std::map<unsigned long, unsigned long> {
    // Returns map factor-count
    std::map<unsigned long, unsigned long> factors;
    auto const primes = generate_primes(x + 1ul);
    for (auto prime : primes) {
        auto y = x;
        while (y % prime == 0ul) {
            factors[prime]++;
            y /= prime;
        }
    }
    return factors;
}

auto calculate_lcm(std::vector<unsigned long> const& xs) -> unsigned long {
    std::map<unsigned long, unsigned long> factors;
    for (auto x : xs) {
        for (auto [factor, count] : prime_factors(x)) {
            if (!factors.contains(factor)) {
                factors[factor] = count;
            } else if (factors[factor] < count) {
                factors[factor] = count;
            }
        }
    }
    return std::transform_reduce(
        factors.begin(), factors.end(), 1ul, std::multiplies{},
        [](auto const& entry) {
            auto res = 1ul;
            for (auto i = 0ul; i < entry.second; ++i) {
                res *= entry.first;
            }
            return res;
        });
}

auto count_steps_2(Atlas const& atlas, Steps steps, Id const& id)
    -> unsigned long {
    auto node = id;
    while (!is_finish_pos_pt2(node)) {
        if (steps.get_step() == 'L') {
            node = atlas.at(node).first;
        } else {
            node = atlas.at(node).second;
        }
    }
    return steps.get_count();
}

auto calculate_steps_2(Atlas const& atlas, Steps steps) -> unsigned long {
    auto starts = generate_starting_positions(atlas, is_starting_pos_pt2);
    std::vector<unsigned long> paths;
    std::transform(
        starts.begin(), starts.end(), std::back_inserter(paths),
        [&atlas, &steps](Id const& id) {
            return count_steps_2(atlas, steps, id);
        });
    for (auto x : paths) {
        std::cout << "path: " << x << "\n";
    }
    return calculate_lcm(paths);
}

auto main() -> int {
    auto [steps, atlas] = parse(std::cin);
    print(atlas);
    // std::cout << calculate_steps(atlas, steps, is_starting_pos,
    // is_finish_pos) << std::endl;
    std::cout << calculate_steps_2(atlas, steps) << std::endl;
    return 0;
}
