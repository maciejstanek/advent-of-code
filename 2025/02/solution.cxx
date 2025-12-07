#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <cassert>

using Int = unsigned long long;
using Pair = std::pair<Int, Int>;
using Pairs = std::vector<Pair>;

auto parse_pair(std::string const& input) -> Pair {
    auto hyphen_pos = input.find('-');
    assert(hyphen_pos != std::string::npos);
    auto left = input.substr(0, hyphen_pos);
    auto is_last_char_endl = input[input.size() - 1] == '\n';
    auto endpos_offset = is_last_char_endl ? 2 : 1;
    auto right = input.substr(hyphen_pos + 1, input.size() - hyphen_pos - endpos_offset);
    return {std::stoll(left), std::stoll(right)};
}

auto parse(std::istream& input) -> Pairs {
    Pairs result;
    std::string pair_str;
    while (std::getline(input, pair_str, ',')) {
        result.push_back(parse_pair(pair_str));
    }
    return result;
}

auto invalid(Int val) -> bool
{
    const auto str = std::to_string(val);
    return (str.size() % 2 == 1) ? false : (str.substr(0, str.size() / 2 ) == str.substr(str.size() / 2));
}

auto solve1(Pair const& p) -> Int
{
    Int sum = 0;
    for (auto i = p.first; i <= p.second; ++i) {
        if (invalid(i)) {
            sum += i;
        }
    }
    return sum;
}

auto solve1(Pairs const& pairs) -> Int
{
    return std::transform_reduce(pairs.begin(), pairs.end(), Int{}, std::plus{}, [](Pair const& p) -> Int {
        return solve1(p);
    });
}

auto main() -> int {
    const auto x = parse(std::cin);
    std::cout << solve1(x) << std::endl;
    return 0;
}
