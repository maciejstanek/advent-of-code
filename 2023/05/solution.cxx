#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

using Line = std::string;
using Lines = std::vector<Line>;

auto parse_lines(std::istream& input) -> Lines {
    Lines lines;
    Line line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    return lines;
}

using Value = long;
using Values = std::vector<Value>;

auto parse_numbers(Line::const_iterator begin, Line::const_iterator end)
    -> Values {
    Values numbers;
    auto const is_digit = [](char c) { return std::isdigit(c); };
    auto current = std::find_if(begin, end, is_digit);
    while (current != end) {
        auto current_end = std::find_if_not(current, end, is_digit);
        numbers.push_back(std::stol(std::string(current, current_end)));
        current = std::find_if(current_end, end, is_digit);
    }
    return numbers;
}

struct Span {
    Value start;
    Value length;
    Value end() const { return start + length; }
};
using Span_opt = std::optional<Span>;
using Spans = std::vector<Span>;
using Span_opts = std::vector<Span_opt>;
struct Named_spans {
    std::string name;
    Spans spans;
};
struct Entry {
    Value to;
    Value from;
    Value length;
    Value from_end() const { return from + length; }
    Value offset() const { return to - from; }
};
using Entries = std::vector<Entry>;
struct Map {
    std::string to;
    Entries entries;
};
using Almanac = std::map<std::string, Map>;
struct Named_values {
    std::string name;
    Values values;
};

auto interpret_as_values(Values const& values) -> Spans {
    Spans spans;
    spans.reserve(values.size());
    std::transform(
        values.begin(), values.end(), std::back_inserter(spans),
        [](Value value) -> Span {
            return {value, 1};
        });
    return spans;
}

auto interpret_as_spans(Values const& values) -> Spans {
    Spans spans;
    assert(values.size() % 2 == 0);
    spans.reserve(values.size() / 2);
    // Missing std::ranges::to and std::ranges::views::chunk
    for (auto i = values.begin(); i != values.end(); i += 2) {
        spans.emplace_back(*i, *std::next(i));
    }
    return spans;
}

auto parse_seeds(Line const& line, auto interpretor) -> Named_spans {
    auto const seeds_begin = std::find(line.begin(), line.end(), ':');
    assert(seeds_begin != line.end());
    return Named_spans{
        "seed", interpretor(parse_numbers(seeds_begin, line.end()))};
}

auto parse_map(Lines::const_iterator begin, Lines::const_iterator end)
    -> std::pair<std::string, Map> {
    assert(begin != end);
    auto const& header = *begin;
    auto const first_dash = std::find(header.begin(), header.end(), '-');
    auto const second_dash =
        std::find(std::next(first_dash), header.end(), '-');
    auto const space = std::find(second_dash, header.end(), ' ');
    auto const from = std::string(header.begin(), first_dash);
    auto const to = std::string(std::next(second_dash), space);

    Entries entries;
    ++begin;
    while (begin != end) {
        auto values = parse_numbers(begin->begin(), begin->end());
        assert(values.size() == 3);
        entries.emplace_back(values[0], values[1], values[2]);
        ++begin;
    }

    return std::make_pair(from, Map{to, entries});
}

auto interpret_lines(std::vector<std::string> const& lines)
    -> std::tuple<Named_spans, Named_spans, Almanac> {
    Almanac almanac{};
    Named_spans seeds{};
    Named_spans seed_spans{};
    auto current = lines.begin();
    while (current != lines.end()) {
        if (current->starts_with("seeds:")) {
            seeds = parse_seeds(
                *current, [](auto x) { return interpret_as_values(x); });
            seed_spans = parse_seeds(
                *current, [](auto x) { return interpret_as_spans(x); });
            ++current;
        } else if (current->find("map:") != std::string::npos) {
            auto const is_empty = [](std::string const& line) {
                return line.empty();
            };
            auto const end = find_if(current, lines.end(), is_empty);
            almanac.emplace(parse_map(current, end));
            current = end;
        } else if (current->empty()) {
            ++current;
        }
    }
    return {seeds, seed_spans, almanac};
}

auto print(std::ostream& out, Named_spans const& spans) -> void {
    out << spans.name << ":";
    for (auto span : spans.spans) {
        out << " [" << span.start << ":" << span.start + span.length - 1 << "]";
    }
    out << '\n';
}

auto print(std::ostream& out, Almanac const& almanac) -> void {
    for (auto const& [x, y] : almanac) {
        out << "FROM " << x;
        out << " TO " << y.to << "\n";
        for (auto const& z : y.entries) {
            out << "  [" << z.from << ":" << z.from + z.length - 1 << "] -> ["
                << z.to << ":" << z.to + z.length - 1 << "]\n";
        }
    }
}

auto translate_span(Span const& span, Entry const& entry)
    -> std::pair<Span_opt, Span_opt> {
    auto const start = std::max(span.start, entry.from);
    auto const end = std::min(span.end(), entry.from_end());
    return end > start ? std::make_pair(
                             Span_opt{{start, end - start}},
                             Span_opt{{start + entry.offset(), end - start}})
                       : std::make_pair(Span_opt{}, Span_opt{});
}

auto translate_span(Span const& span, Entries const& entries)
    -> std::pair<Spans, Spans> {
    Spans sources;
    Spans translations;
    for (auto const& entry : entries) {
        auto [source, translation] = translate_span(span, entry);
        if (source.has_value()) {
            sources.push_back(*source);
        }
        if (translation.has_value()) {
            translations.push_back(*translation);
        }
    }
    return std::make_pair(sources, translations);
}

auto make_spans_inverse(Spans matches, Span full_span) -> Spans {
    Spans spans;
    std::sort(matches.begin(), matches.end(), [](Span const& a, Span const& b) {
        return a.start < b.start;
    });
    auto start = full_span.start;
    for (auto const& match : matches) {
        auto const length = match.start - start;
        if (length > 0) {
            spans.emplace_back(start, length);
        }
        start = match.start + match.length;
    }
    if (start < full_span.start + full_span.length) {
        spans.emplace_back(start, full_span.length + full_span.start - start);
    }
    return spans;
}

auto resolve_mapping(Entries const& entries, Span span) -> Spans {
    Spans spans;
    auto const [matches, translations] = translate_span(span, entries);
    spans.reserve(2 * translations.size() + 1);
    std::copy(
        translations.begin(), translations.end(), std::back_inserter(spans));
    auto const inverses = make_spans_inverse(matches, span);
    std::copy(inverses.begin(), inverses.end(), std::back_inserter(spans));

    return spans;
}

auto resolve_mapping(Entries const& entries, Spans const& spans) -> Spans {
    Spans results{};
    for (auto const& span : spans) {
        // One span can be mapped to multiple spans
        auto partial = resolve_mapping(entries, span);
        results.insert(results.begin(), partial.begin(), partial.end());
    }
    return results;
}

auto resolve_mappings(Almanac const& almanac, Named_spans spans)
    -> Named_spans {
    while (almanac.count(spans.name)) {
        auto const& entry = almanac.at(spans.name);
        spans.spans = resolve_mapping(entry.entries, spans.spans);
        spans.name = entry.to;
    }
    return spans;
}

auto calculate_min(Spans const& spans) -> Value {
    return std::min_element(
               spans.begin(), spans.end(),
               [](Span const& a, Span const& b) { return a.start < b.start; })
        ->start;
}

auto main() -> int {
    auto const [seeds, seed_spans, almanac] =
        interpret_lines(parse_lines(std::cin));
    print(std::cout, seeds);
    print(std::cout, seed_spans);
    print(std::cout, almanac);
    auto const resolved = resolve_mappings(almanac, seeds);
    print(std::cout, resolved);
    std::cout << calculate_min(resolved.spans) << std::endl;
    auto const resolved2 = resolve_mappings(almanac, seed_spans);
    print(std::cout, resolved2);
    std::cout << calculate_min(resolved2.spans) << std::endl;

    return 0;
}
