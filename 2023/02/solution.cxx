#include <algorithm>
#include <iostream>
#include <numeric>
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

struct Pull {
    int r, g, b;
};
using Pulls = std::vector<Pull>;

struct Game {
    int id;
    Pulls pulls;
};
using Games = std::vector<Game>;

auto parse_pull(Line const& str) -> Pull {
    Pull pull{};
    auto iter = str.begin();
    while (iter != str.end()) {
        auto num_iter = std::find_if(iter, str.end(), static_cast<int (*)(int)>(std::isdigit));
        auto num_iter_end = std::find(num_iter, str.end(), ' ');
        auto type_iter =
            std::find_if(num_iter_end, str.end(), static_cast<int (*)(int)>(std::isalpha));
        auto num = std::atoi(std::string{num_iter, num_iter_end}.c_str());
        switch (*type_iter) {
            case 'r': {
                pull.r = num;
                break;
            }
            case 'g': {
                pull.g = num;
                break;
            }
            case 'b': {
                pull.b = num;
                break;
            }
        }
        iter = std::find(iter + 1, str.end(), ',');
    }
    return pull;
}

auto parse_games(Lines const& lines) -> Games {
    Games games{};
    for (auto const& line : lines) {
        auto iter = std::find(line.begin(), line.end(), ':');
        auto id_str = std::string(std::next(std::find(line.begin(), iter, ' ')), iter);
        games.push_back({std::atoi(id_str.c_str()), {}});
        while (iter != line.end()) {
            iter = std::find_if(iter, line.end(), static_cast<int (*)(int)>(std::isdigit));
            auto end_iter = std::find(iter, line.end(), ';');
            games.back().pulls.push_back(parse_pull({iter, end_iter}));
            iter = end_iter;
        }
    }
    return games;
}

auto main() -> int {
    auto const games = parse_games(getlines(std::cin));

    auto const is_possible = [](Pull const& pull) {
        auto const max_pull = Pull{12, 13, 14};
        return pull.r <= max_pull.r && pull.g <= max_pull.g && pull.b <= max_pull.b;
    };
    auto const are_possible = [&is_possible](Pulls const& pulls) {
        return std::all_of(pulls.begin(), pulls.end(), is_possible);
    };
    auto const to_factor = [&are_possible](Game const& game) {
        return are_possible(game.pulls) ? game.id : 0;
    };
    auto result = std::transform_reduce(games.cbegin(), games.cend(), 0, std::plus<>(), to_factor);
    std::cout << result << '\n';

    auto min_span = [](Pull const& lhs, Pull const& rhs) -> Pull {
        return {std::max(lhs.r, rhs.r), std::max(lhs.g, rhs.g), std::max(lhs.b, rhs.b)};
    };
    auto total_min_span = [&min_span](Pulls const& pulls) -> Pull {
        return std::reduce(pulls.begin(), pulls.end(), Pull{}, min_span);
    };
    auto const to_factor2 = [&total_min_span](Game const& game) {
        auto total = total_min_span(game.pulls);
        return total.r * total.g * total.b;
    };
    auto result2 =
        std::transform_reduce(games.cbegin(), games.cend(), 0, std::plus<>(), to_factor2);
    std::cout << result2 << '\n';
    return 0;
}
