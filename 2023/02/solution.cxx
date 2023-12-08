#include <algorithm>
#include <iostream>
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

struct Game {
    int id;
    std::vector<Pull> pulls;
};

using Games = std::vector<Game>;

auto parse_pull(Line const& str) -> Pull {
    Pull pull{};
    auto iter = str.begin();
    while (iter != str.end()) {
        auto num_iter = std::find_if(iter, str.end(), static_cast<int(*)(int)>(std::isdigit));
        auto num_iter_end = std::find(num_iter, str.end(), ' ');
        auto type_iter = std::find_if(num_iter_end, str.end(), static_cast<int(*)(int)>(std::isalpha));
        auto num = std::atoi(std::string{num_iter, num_iter_end}.c_str());
        switch (*type_iter) {
            case 'r': {
                pull.r = num;
                std::cout << "R = " << num << "\n";
                break;
            }
            case 'g': {
                pull.g = num;
                std::cout << "G = " << num << "\n";
                break;
            }
            case 'b': {
                pull.b = num;
                std::cout << "B = " << num << "\n";
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
            iter = std::find_if(iter, line.end(), static_cast<int(*)(int)>(std::isdigit));
            auto end_iter = std::find(iter, line.end(), ';');
            games.back().pulls.push_back(parse_pull({iter, end_iter}));
            iter = end_iter;
        }
    }
    return games;
}

auto main() -> int {
    parse_games(getlines(std::cin));
    return 0;
}
