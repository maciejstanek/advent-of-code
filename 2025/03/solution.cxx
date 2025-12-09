#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <cassert>
#include <optional>
#include <execution>

using Int = long long;
using Bank = std::vector<Int>;
using Input = std::vector<Bank>;

auto to_string(Bank const& bank) -> std::string {
    std::string result;
    for (auto x : bank) {
        result += std::to_string(x);
    }
    return result;
}

auto pow(Int val, Int pow) -> Int {
    Int result = 1;
    for (auto i = Int{}; i < pow; ++i) {
        result *= val;
    }
    return result;
}

auto repeat(std::string const& text, Int count) -> std::string {
    std::string result;
    while(--count) {
        result += text;
    }
    return result;
}

// auto solve2(Bank const& bank, Int depth) -> std::optional<Int> {
//     std::cout << "solve2(" << depth << "):" << repeat("    ", 6-depth) << to_string(bank) << " " << depth << "\n";
// 
//     if (depth == 0) {
//         return std::nullopt;
//     }
//     if (bank.size() == 1) {
//         return bank.back();
//     }
//     // if (depth == 1) {
//     //     auto value = std::max_element(bank.begin(), bank.end());
//     //     if (value != bank.end()) {
//     //         return std::optional<Int>(*value);
//     //     }
//     //     return std::nullopt;
//     // }
//     //if (depth - 1 < static_cast<Int>(bank.size())) {
//     //    return std::nullopt;
//     //}
// 
//     auto max_max = Int{};
//     for (auto i = Int(9); i >= Int{}; --i) {
//         //if (depth == 11) { std::cout << i << std::endl; }
//         auto pos = std::find(bank.begin(), bank.end(), i); // first occurence
//         if (pos != bank.end()) {
//             auto remaining = Bank(std::next(pos), bank.end());
//             //std::cout << "DBG: " << i << " " << to_string(substr) << std::endl;
//             //if (remaining.size() > 0) {
//                 const auto next_iter = solve2(remaining, depth - 1);
//                 if (next_iter) {
//                     const auto current_max = i * pow(10, depth - 1) + *next_iter;
//                     std::cout << "solve2(" << depth << "):" << repeat("    ", 6-depth) << "cur_max=" << current_max << "\n";
//                     if (current_max > max_max) {
//                         max_max = current_max;
//                         std::cout << "solve2(" << depth << "):" << repeat("    ", 6-depth) << "max_max=" << max_max << "\n";
//                     }
//                 }
//             //}
//         }
//     }
//     return max_max;
// }

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
        input.begin(), input.end(), Int{}, std::plus{},
        [](Bank const& bank) -> Int { return solve1(bank); });
}

auto solve2(Bank const& bank, Int depth, Int start_value = Int{0}) -> std::optional<Int> {
    if (bank.empty()) {
        return std::nullopt;
    }

    std::optional<Int> max_max;
    // Split into 10 simpler problems one element smaller and compare results
    for (auto i = start_value; i < Int{10}; ++i) {
        const auto iter = std::find(bank.begin(), bank.end(), i);
        if (iter != bank.end()) {
            if (depth == 1) {
                if (max_max) {
                    max_max = std::max(*max_max, i);
                }
                else {
                    max_max = i;
                }
            }
            else {
                const auto subset = Bank(std::next(iter), bank.end());
                const auto subsolution = solve2(subset, depth - 1);
                if (subsolution) {
                    const auto current_max = i * pow(10, depth - 1) + *subsolution;
                    if (max_max) {
                        max_max = std::max(*max_max, current_max);
                    }
                    else {
                        max_max = current_max;
                    }
                }
            }
        }

    }

    return max_max;
}

auto solve2(Input const& input, Int depth) -> Int {
    return std::transform_reduce(
        std::execution::par_unseq,
        input.begin(), input.end(), Int{}, std::plus{},
        [depth](Bank const& bank) -> Int {
            std::cout << to_string(bank) << " started" << std::endl;
            auto x = solve2(bank, depth, 1).value_or(Int{});
            std::cout << to_string(bank) << " ended with " << x << std::endl;
            return x;
        });
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
    std::cout << solve2(input, 12) << std::endl;
    return 0;
}
