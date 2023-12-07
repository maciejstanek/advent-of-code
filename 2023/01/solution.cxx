#include <iostream>
#include <ranges>

int main() {
    std::string line;
    auto isdigit = [](char x) { return std::isdigit(x); };
    auto sum = 0;
    while (std::getline(std::cin, line)) {
        auto tens = *std::ranges::find_if(line, isdigit) - '0';
        auto units = *std::ranges::find_if(line | std::views::reverse, isdigit) - '0';
        sum += 10 * tens + units;
    }
    std::cout << sum << '\n';
}
