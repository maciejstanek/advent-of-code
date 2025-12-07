#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <cassert>

using Int = int;
using Bank = std::vector<Int>;
using Input = std::vector<Bank>;

auto to_string(Bank const& bank) -> std::string {
    std::string result;
    for (auto x : bank) {
        result += std::to_string(x);
    }
    return result;
}

auto solve1(Bank const& bank) -> Int {
    auto current_max = 0;
    for (auto i = 9; i >= 0; --i) {
        auto pos = std::find(bank.begin(), bank.end(), i); // first occurence
        if (pos != bank.end()) {
            auto substr = Bank(std::next(pos), bank.end());
            if (substr.size() > 0) {
                auto value2 = *std::max_element(substr.begin(), substr.end());
                auto new_max = i * 10 + value2;
                if (new_max > current_max) {
                    current_max = new_max;
                }
            }
        }
    }
    return current_max;
}

auto solve1(Input const& input) -> Int {
    return std::transform_reduce(
        input.begin(), input.end(), 0, std::plus{},
        [](Bank const& bank) -> Int { return solve1(bank); });
}

auto parse(std::string const& line) -> Bank {
    Bank bank;
    bank.reserve(line.size());
    std::transform(line.begin(), line.end(), std::back_inserter(bank), [](char c) -> Int { return c - '0'; });
    return bank;
}

auto parse(std::istream& input) -> Input {
    Input result;
    std::string line;
    while (std::getline(input, line)) {
        result.push_back(parse(line));
    }
    return result;
}

auto main() -> int {
    const auto input = parse(std::cin);
    std::cout << solve1(input) << std::endl;
    return 0;
}
