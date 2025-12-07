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

auto invalid1(Int val) -> bool
{
    const auto str = std::to_string(val);
    return (str.size() % 2 == 1) ? false : (str.substr(0, str.size() / 2 ) == str.substr(str.size() / 2));
}

auto LIMIT = 20;

auto is_repetition_of(std::string const& text, std::string const& atom) -> bool
{
    if (text.length() % atom.length()) {
        return false;
    }
    for (auto i = atom.length(); i < text.length(); i += atom.length()) {
        if (atom != text.substr(i, atom.length())) {
            return false;
        }
    }
    return true;
}

auto invalid2(Int val) -> bool
{
    const auto str = std::to_string(val);
    for (auto current_length = 1ull; current_length < str.length(); ++current_length) {
        auto atom = str.substr(0, current_length);
        if (is_repetition_of(str, atom)) {
            return true;
        }
    }
    return false;
}

auto solve2(Pair const& p) -> Int
{
    Int sum = 0;
    for (auto i = p.first; i <= p.second; ++i) {
        if (invalid2(i)) {
            sum += i;
        }
    }
    return sum;
}

auto solve1(Pair const& p) -> Int
{
    Int sum = 0;
    for (auto i = p.first; i <= p.second; ++i) {
        if (invalid1(i)) {
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

auto solve2(Pairs const& pairs) -> Int
{
    return std::transform_reduce(pairs.begin(), pairs.end(), Int{}, std::plus{}, [](Pair const& p) -> Int {
        return solve2(p);
    });
}

auto main() -> int {
    const auto x = parse(std::cin);
    std::cout << solve1(x) << std::endl;
    std::cout << solve2(x) << std::endl;
    return 0;
}
