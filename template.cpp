#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "json.hpp"

using json = nlohmann::json;
const int INF = 1e9;

struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
    }
};

std::pair<int, std::vector<int>> heldKarp(const std::vector<std::vector<int>>& dist) {
    int n = dist.size();
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> dp;

    // Исправлено: маска включает стартовую вершину (0)
    for (int i = 1; i < n; ++i) {
        dp[{ (1 << 0) | (1 << i), i }] = {dist[0][i], 0};
    }

    // Основной цикл ДП
    for (int mask = 1; mask < (1 << n); ++mask) {
        for (int last = 0; last < n; ++last) {
            if (!(mask & (1 << last))) continue;
            if (!dp.count({mask, last})) continue;

            for (int next = 0; next < n; ++next) {
                if (mask & (1 << next)) continue;
                int newMask = mask | (1 << next);
                int newCost = dp[{mask, last}].first + dist[last][next];

                if (!dp.count({newMask, next}) || newCost < dp[{newMask, next}].first) {
                    dp[{newMask, next}] = {newCost, last};
                }
            }
        }
    }

    // Поиск оптимального пути
    int minCost = INF;
    int lastNode = -1;
    int fullMask = (1 << n) - 1;

    for (int i = 1; i < n; ++i) {
        if (dp.count({fullMask, i}) && (dp[{fullMask, i}].first + dist[i][0] < minCost)) {
            minCost = dp[{fullMask, i}].first + dist[i][0];
            lastNode = i;
        }
    }

    // Восстановление пути с проверкой
    std::vector<int> path;
    int currentMask = fullMask;
    int current = lastNode;

    while (current != 0) {
        path.push_back(current);
        int prev = dp[{currentMask, current}].second;
        currentMask ^= (1 << current); // Исправлено: правильное удаление вершины
        current = prev;
    }

    path.push_back(0);
    std::reverse(path.begin(), path.end());
    path.push_back(0); // Замыкаем цикл

    return {minCost, path};
}

int main() {
    std::ifstream in("matrix_input.json");
    if (!in) {
        std::cerr << "Ошибка чтения файла";
        return 1;
    }

    json j;
    in >> j;
    auto matrix = j.get<std::vector<std::vector<int>>>();

    int n = matrix.size();
    for (auto& row : matrix) {
        for (auto& val : row) {
            if (val == -1) val = INF;
        }
    }

    auto [cost, path] = heldKarp(matrix);

    // Преобразование в 1-based
    for (auto& node : path) node += 1;

    json output = {
        {"cost", cost},
        {"path", path}
    };
    std::cout << output.dump() << std::endl;

    return 0;
}