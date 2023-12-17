#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

using Line = std::string;
using Line_iter = Line::const_iterator;
using Value = long;
using Values = std::vector<Value>;

auto parse_values(Line_iter begin, Line_iter end) -> std::pair<Values, Value> {
    Values values;
    Line concated;
    while (begin != end) {
        begin =
            std::find_if(begin, end, [](auto x) { return std::isdigit(x); });
        if (begin == end) {
            break;
        }
        auto const after = std::find(begin, end, ' ');
        auto part = std::string(begin, after);
        values.push_back(std::stol(part));
        concated += part;
        begin = after;
    }
    return std::make_pair(values, std::stol(concated));
}

auto parse(std::istream& input)
    -> std::pair<std::pair<Values, Values>, std::pair<Value, Value>> {
    Line time_str;
    Line distance_str;
    std::getline(input, time_str);
    std::getline(input, distance_str);
    auto [times, time] = parse_values(
        std::find(time_str.begin(), time_str.end(), ':') + 1, time_str.end());
    auto [distances, distance] = parse_values(
        std::find(distance_str.begin(), distance_str.end(), ':') + 1,
        distance_str.end());
    return std::make_pair(
        std::make_pair(times, distances), std::make_pair(time, distance));
}

auto calculate_distance_from_charge_and_total_time(
    Value charge_time, Value total_time) -> Value {
    // When I think about it, it should be possible to solve it with math
    return charge_time * (total_time - charge_time);
}

struct Race {
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = Value;
        using value_type = Value;
        using pointer = Value;
        using reference = Value&;
        value_type operator*() const {
            return calculate_distance_from_charge_and_total_time(
                time, race->time);
        }

        // Prefix increment
        Iterator& operator++() {
            time++;
            return *this;
        }

        // Postfix increment
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(Iterator const& a, Iterator const& b) {
            return a.time == b.time;
        };
        // c++20 has implicit != from ==
        Race* race;
        pointer time;
    };
    Value time;
    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, time); }
};

auto calculate_number_of_ways_to_win(Value time, Value distance) -> Value {
    auto race = Race{time};
    auto const peak = std::max_element(race.begin(), race.end());
    auto const left_pos = std::upper_bound(race.begin(), peak, distance);
    auto const right_pos =
        std::upper_bound(peak, race.end(), distance, std::greater_equal{});
    return right_pos.time - left_pos.time;
}

auto main() -> int {
    auto const [pt1, pt2] = parse(std::cin);
    auto const [times, distances] = pt1;
    auto const [time, distance] = pt2;
    Values results;
    results.reserve(times.size());
    std::transform(
        times.begin(), times.end(), distances.begin(),
        std::back_inserter(results), [](Value time, Value distance) {
            return calculate_number_of_ways_to_win(time, distance);
        });
    auto result =
        std::accumulate(results.begin(), results.end(), 1, std::multiplies{});
    std::cout << result << std::endl;
    std::cout << calculate_number_of_ways_to_win(time, distance) << std::endl;
    return 0;
}
