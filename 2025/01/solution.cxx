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
    const auto x = parse(std::cin);

    std::vector<int> y;
    y.reserve(x.size());
    const auto add_modulo_100 = [](int a, int b) {
        return (a + b + 200) % 100;
    };
    std::inclusive_scan(x.begin(), x.end(), std::back_inserter(y), add_modulo_100, 50);
    std::cout << std::count(y.begin(), y.end(), 0) << '\n';

    std::vector<int> z;
    z.reserve(x.size());
    auto count = 0;
    const auto part2 = [&](int pos, int turn) {
        if (turn > 0) {
            count += turn / 100;
            turn = (turn % 100);

            if (turn + pos >= 100) {
                count += 1;
            }
        }
        if (turn < 0) {
            // Treat left turns as right turns but reverse them
            turn = -turn;
            if (pos > 0) {
                pos = 100 - pos;
            }

            count += turn / 100;
            turn = (turn % 100);

            if (turn + pos >= 100) {
                count += 1;
            }

            // Invert back
            turn = -turn;
            if (pos > 0) {
                pos = 100 - pos;
            }
        }
        return (pos + turn + 200) % 100;
    };
    std::inclusive_scan(x.begin(), x.end(), std::back_inserter(z), part2, 50);
    std::cout << "verif p1: " << std::count(y.begin(), y.end(), 0) << '\n';
    std::cout << "p2: " << count << '\n';

    return 0;
}
