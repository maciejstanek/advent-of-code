#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

using Int = long;
using Ints = std::vector<Int>;
using Input = std::vector<Ints>;

auto parse_numbers(std::string const& line) -> Ints {
    Ints ints;
    auto i = line.begin();
    while (i != line.end()) {
        auto j = std::find(i, line.end(), ' ');
        ints.push_back(std::stol(std::string(i, j)));
        i = std::find_if(j, line.end(), [](char c) { return c != ' '; });
    }
    return ints;
}

auto parse(std::istream& input) -> Input {
    Input parsed;
    std::string line;
    while (std::getline(input, line)) {
        parsed.push_back(parse_numbers(line));
    }
    return parsed;
}

auto debug_print(char const* prefix, Ints const& line) -> void {
    std::cout << prefix << ":";
    for (auto value : line) {
        std::cout << " (" << value << ")";
    }
    std::cout << "\n";
}

auto debug_print(char const* prefix, Input const& input) -> void {
    for (const auto& line : input) {
        debug_print(prefix, line);
    }
}

auto predict(Ints ints) -> Int {
    auto const last = ints.back();
    auto const equal = [&last](Int x) { return x == last; };
    if (std::ranges::all_of(ints, equal)) {
        return last;
    }
    std::transform(
        std::next(ints.begin()), ints.end(), ints.begin(), ints.begin(),
        [](Int const& a, Int const& b) { return a - b; });
    ints.erase(std::prev(ints.end()), ints.end());
    return last + predict(ints);
}

auto main() -> int {
    auto input = parse(std::cin);
    /* debug_print("LINE", input); */
    Ints results;
    results.reserve(input.size());
    std::transform(
        input.begin(), input.end(), std::back_inserter(results), predict);
    /* debug_print("PARTIAL", results); */
    std::cout << std::accumulate(results.begin(), results.end(), 0)
              << std::endl;
    return 0;
}
