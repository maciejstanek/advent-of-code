#include <algorithm>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

using Line = std::string;
using Lines = std::vector<Line>;

auto getlines(std::istream& input) -> Lines {
    Lines lines;
    Line line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    return lines;
}

auto extend_parse(
    std::string::const_iterator begin, std::string::const_iterator iter,
    std::string::const_iterator end) -> int {
    auto const is_not_digit = [](char c) { return !std::isdigit(c); };
    auto const right = std::find_if(iter, end, is_not_digit);
    auto const left = std::find_if(
                          std::make_reverse_iterator(iter),
                          std::make_reverse_iterator(begin), is_not_digit)
                          .base();
    return std::stoi(std::string{left, right});
}

struct Part {
    Line::const_iterator x;
    Lines::const_iterator y;
    auto symbol() const -> char { return *x; }
};
using Parts = std::vector<Part>;

auto find_parts(Lines const& lines) -> Parts {
    Parts parts;
    for (auto line_iter = lines.begin(); line_iter != lines.end();
         ++line_iter) {
        auto const is_part = [](char c) {
            return !std::isdigit(c) && c != '.';
        };
        auto part_iter =
            std::find_if(line_iter->begin(), line_iter->end(), is_part);
        while (part_iter != line_iter->end()) {
            parts.emplace_back(part_iter, line_iter);
            part_iter =
                std::find_if(std::next(part_iter), line_iter->end(), is_part);
        }
    }
    return parts;
}

using Label = int;
using Label_opt = std::optional<Label>;
using Side = std::pair<Label_opt, Label_opt>;
using Labels = std::vector<int>;
struct Labeled_part {
    Part part;
    Labels labels;
};
using Labeled_parts = std::vector<Labeled_part>;

auto find_label_left(Line const& line, Line::const_iterator iter) -> Label_opt {
    return (iter != line.begin() && std::isdigit(*std::prev(iter)))
               ? Label_opt{extend_parse(
                     line.begin(), std::prev(iter), line.end())}
               : std::nullopt;
}

auto find_label_right(Line const& line, Line::const_iterator iter)
    -> Label_opt {
    return (std::next(iter) != line.end() && std::isdigit(*std::next(iter)))
               ? Label_opt{extend_parse(
                     line.begin(), std::next(iter), line.end())}
               : std::nullopt;
}

auto find_label_center(Line const& line, Line::const_iterator iter)
    -> Label_opt {
    return extend_parse(line.begin(), iter, line.end());
}

auto find_labels(Line const& line, Line::const_iterator iter) -> Side {
    return (std::isdigit(*iter))
               ? std::make_pair(find_label_center(line, iter), std::nullopt)
               : std::make_pair(
                     find_label_left(line, iter), find_label_right(line, iter));
}

auto find_labels(Lines const& lines, Part const& part) -> Labeled_part {
    Labels labels{};
    auto const accumulate = [&labels](Side const& side) {
        auto const val0 = std::get<0>(side);
        if (val0.has_value()) {
            labels.push_back(*val0);
        }
        auto const val1 = std::get<1>(side);
        if (val1.has_value()) {
            labels.push_back(*val1);
        }
    };
    // Compare previous to current
    if (part.y != lines.begin()) {
        auto const prev = *std::prev(part.y);
        accumulate(find_labels(
            prev, prev.begin() + std::distance(part.y->begin(), part.x)));
    }
    // Find in the current
    accumulate(find_labels(*part.y, part.x));
    // Compare to the next
    if (std::next(part.y) != lines.end()) {
        auto const next = *std::next(part.y);
        accumulate(find_labels(
            next, next.begin() + std::distance(part.y->begin(), part.x)));
    }
    return {part, labels};
}

auto find_labels(Lines const& lines, Parts const& parts) -> Labeled_parts {
    Labeled_parts result;
    result.reserve(parts.size());
    std::transform(
        parts.cbegin(), parts.cend(), std::back_inserter(result),
        [&lines](Part const& part) { return find_labels(lines, part); });
    return result;
}

auto main() -> int {
    auto const lines = getlines(std::cin);
    // This would be a perfect place to use the S combinator (Sxyz = xz(yz)) to
    // eliminate the use of the `lines` temporary variable.
    auto const parts = find_labels(lines, find_parts(lines));

    auto const calc_part_sum = [](Labeled_part const& part) {
        return std::accumulate(part.labels.begin(), part.labels.end(), 0);
    };
    auto const sum = std::transform_reduce(
        parts.begin(), parts.end(), 0, std::plus{}, calc_part_sum);
    std::cout << sum << '\n';
    return 0;
}
