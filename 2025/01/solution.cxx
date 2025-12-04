#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

auto parse(std::string value) -> int {
    value[0] = (value[0] == 'R') ? '+' : '-';
    return std::stoi(value);
}

auto parse(std::istream& input) -> std::vector<int> {
    std::vector<int> result;
    std::string line;
    while (std::getline(input, line)) {
        result.push_back(parse(line));
    }
    return result;
}

auto main() -> int {
    auto x = parse(std::cin);
    constexpr const auto add_modulo_100 = [](int a, int b) {
        return (a + b + 200) % 100;
    };
    std::inclusive_scan(x.begin(), x.end(), x.begin(), add_modulo_100, 50);
    std::cout << std::count(x.begin(), x.end(), 0) << '\n';
    return 0;
}
