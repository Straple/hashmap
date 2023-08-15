#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>
#include "contrib/catch/catch.hpp"
#include "hashmap.hpp"

using namespace std::chrono;

using Key = std::pair<std::size_t, std::size_t>;

namespace std {

template <>
struct hash<Key> {
    [[nodiscard]] std::size_t operator()(const Key &value) const noexcept {
        return (value.first * 1238227321 + value.second) ^
               (value.first * value.second);
    }
};

}  // namespace std

template <class Map>
std::size_t my_gcd(std::size_t a, std::size_t b, Map &dp) {
    if (a == 0) {
        return b;
    }
    // small boost
    /*else if (a > b) {
        return my_gcd(b, a, dp);
    }*/
    else if (dp.count({a, b})) {
        return dp[{a, b}];
    } else {
        return dp[{a, b}] = my_gcd(b % a, a, dp);
    }
}

// returns (mean time, max time)
template <class Map>
std::pair<nanoseconds, nanoseconds> RunStress(
    Map &map,
    int seed,
    std::size_t max_value,
    std::vector<std::size_t> &responses
) {
    std::vector<Key> queries;
    for (std::size_t a = 1; a <= max_value; a++) {
        for (std::size_t b = 1; b <= max_value; b++) {
            queries.emplace_back(a, b);
        }
    }

    std::mt19937 gen(seed);
    for (int step = 0; step < 100; step++) {
        std::shuffle(queries.begin(), queries.end(), gen);
    }

    // calc duration
    nanoseconds duration;
    nanoseconds max = {};
    {
        responses.reserve(queries.size());

        auto start = steady_clock::now();
        for (auto [a, b] : queries) {
            auto start_query = steady_clock::now();
            responses.push_back(my_gcd(a, b, map));
            auto stop_query = steady_clock::now();
            max = std::max(
                max, duration_cast<nanoseconds>(stop_query - start_query)
            );
        }
        auto stop = steady_clock::now();
        duration = stop - start;
    }
    return std::make_pair(duration, max);
}

template <typename T>
struct hasher_counter {
    mutable std::size_t counter = 0;

    std::size_t operator()(const T &value) const {
        counter++;
        return std::hash<T>{}(value);
    }
};

template <typename Map>
struct tester {
    std::vector<std::size_t> responses;
    nanoseconds duration_time = {}, max_time = {};
    std::size_t hash_calculation_counter = 0;

    const static std::size_t max_value = 10;
    const static int seed = 42;

    tester() {
        hasher_counter<Key> hasher;
        Map map(max_value * max_value, std::reference_wrapper(hasher));
        auto [calc_duration, calc_max] =
            RunStress(map, seed, max_value, responses);
        duration_time = calc_duration;
        max_time = calc_max;
        hash_calculation_counter = hasher.counter;
    }

    // for std::map
    tester(int) {
        Map map;
        auto [calc_duration, calc_max] =
            RunStress(map, seed, max_value, responses);
        duration_time = calc_duration;
        max_time = calc_max;
    }

    void print() {
        std::cerr << "mean "
                  << duration_cast<nanoseconds>(duration_time).count() /
                         (max_value * max_value)
                  << " ns, max " << max_time.count()
                  << " ns/iter, hash calc counter " << hash_calculation_counter
                  << "\n";
    }
};

TEST_CASE("super stress and benchmark") {
    std::vector<std::size_t> stl_map_responses;
    {
        tester<std::map<Key, std::size_t>> tester(0);
        stl_map_responses = tester.responses;

        std::cerr << "std::map, ";
        tester.print();
    }

    std::vector<std::size_t> stl_hashmap_responses;
    {
        tester<std::unordered_map<
            Key, std::size_t, std::reference_wrapper<hasher_counter<Key>>>>
            tester;

        stl_hashmap_responses = tester.responses;

        std::cerr << "std::unordered_map, ";
        tester.print();
    }

    std::vector<std::size_t> my_hashmap_responses;
    {
        tester<hashmap<
            Key, std::size_t, std::reference_wrapper<hasher_counter<Key>>>>
            tester;

        my_hashmap_responses = tester.responses;

        std::cerr << "hashmap, ";
        tester.print();
    }

    REQUIRE(stl_map_responses == stl_hashmap_responses);
    REQUIRE(my_hashmap_responses == stl_map_responses);
    REQUIRE(my_hashmap_responses == stl_hashmap_responses);
}

/*
100x100
std::map, mean 536 ns, max 119081 ns/iter, hash calc counter 0
std::unordered_map, mean 137 ns, max 15129 ns/iter, hash calc counter 39036
hashmap, mean 116 ns, max 2771 ns/iter, hash calc counter 49036

1000x1000
std::map, mean 1857 ns, max 558052 ns/iter, hash calc counter 0
std::unordered_map, mean 507 ns, max 155279 ns/iter, hash calc counter 3985862
hashmap, mean 231 ns, max 132371 ns/iter, hash calc counter 4985862

2000x2000
std::map, mean 2943 ns, max 553130 ns/iter, hash calc counter 0
std::unordered_map, mean 608 ns, max 1159536 ns/iter, hash calc counter 16745342
hashmap, mean 268 ns, max 197862 ns/iter, hash calc counter 20939646
 */