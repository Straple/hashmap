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

template <typename K, typename V>
using map_t = hashmap<K, V>;

// returns (mean time, max time)
template <class Map>
std::pair<nanoseconds, nanoseconds> RunStress(
    Map &map,
    int seed,
    std::size_t elems_count,
    std::size_t iterations,
    std::vector<int> &responses
) {
    responses.reserve(iterations);

    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(0, elems_count);
    for (std::size_t i = 0; i < elems_count; i++) {
        map.insert({dist(gen), {}});
    }

    auto start = steady_clock::now();
    for (std::size_t i = 0; i < iterations; i++) {
        auto val = dist(gen);
        auto resp = map.erase(val);
        responses.push_back(resp);
        if (resp) {
            map.insert({val, {}});
        }
    }
    auto stop = steady_clock::now();
    auto duration = stop - start;

    nanoseconds max = {};
    for (std::size_t i = 0; i < iterations; i++) {
        auto val = dist(gen);
        auto start = steady_clock::now();
        if (map.erase(val)) {
            map.insert({val, {}});
        }
        auto stop = steady_clock::now();
        max = std::max(max, duration_cast<nanoseconds>(stop - start));
    }
    return std::make_pair(duration, max);
}

struct Data {
    double d[3];
};

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
    std::vector<int> responses;
    nanoseconds duration_time = {}, max_time = {};
    std::size_t hash_calculation_counter = 0;

    const static std::size_t elems_count = 1e4;
    const static std::size_t iterations = 1e5;
    const static int seed = 42;

    tester() {
        hasher_counter<int> hasher;
        Map map(elems_count, std::reference_wrapper(hasher));
        auto [calc_duration, calc_max] =
            RunStress(map, seed, elems_count, iterations, responses);
        duration_time = calc_duration;
        max_time = calc_max;
        hash_calculation_counter = hasher.counter;
    }

    // for std::map
    tester(int) {
        Map map;
        auto [calc_duration, calc_max] =
            RunStress(map, seed, elems_count, iterations, responses);
        duration_time = calc_duration;
        max_time = calc_max;
    }

    void print() {
        std::cerr << "mean "
                  << duration_cast<nanoseconds>(duration_time).count() /
                         iterations
                  << " ns, max " << max_time.count()
                  << " ns/iter, hash calc counter " << hash_calculation_counter
                  << "\n";
    }
};

TEST_CASE("super stress and benchmark") {
    {
        tester<std::map<int, Data>> tester(0);

        std::cerr << "std::map, ";
        tester.print();
    }

    std::vector<int> ok_responses;
    {
        tester<std::unordered_map<
            int, Data, std::reference_wrapper<hasher_counter<int>>>>
            tester;

        ok_responses = tester.responses;

        std::cerr << "std::unordered_map, ";
        tester.print();
    }

    {
        tester<hashmap<int, Data, std::reference_wrapper<hasher_counter<int>>>>
            tester;

        REQUIRE(ok_responses == tester.responses);

        std::cerr << "hashmap, ";
        tester.print();
    }
}