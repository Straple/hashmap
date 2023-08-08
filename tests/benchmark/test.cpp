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

struct noncopyable {
    int x = 0;

    noncopyable(int x) : x(x) {
    }

    noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

    noncopyable(noncopyable &&) = default;
    noncopyable &operator=(noncopyable &&) = default;

    friend bool operator==(const noncopyable &lhs, const noncopyable &rhs) {
        return lhs.x == rhs.x;
    }
};

TEST_CASE("noncopyable key and value") {
    // должно компилироваться

    {
        hashmap<int, noncopyable> map;
        map.insert(10, noncopyable(5));
        noncopyable x(5);
        map.insert(10, std::move(x));
        map.insert(std::make_pair(10, noncopyable(5)));
        map.insert(std::make_pair(10, std::move(x)));

        map.erase(10);

        map[5] = std::move(x);
        map[5] = 10;
    }

    {
        struct hasher {
            std::size_t operator()(const noncopyable &x) const {
                return x.x;
            }
        };

        hashmap<noncopyable, int, hasher> map;
        map.insert(noncopyable(5), 10);
        noncopyable x(5);
        map.insert(std::move(x), 10);

        map.insert(std::make_pair(noncopyable(5), 10));
        map.insert(std::make_pair(std::move(x), 10));

        map.erase(5);
        map.erase(x);
        map.erase(std::move(x));

        // CE, but should work. need TODO
        map[noncopyable(5)] = 10;
        map[std::move(x)] = 10;

        // map[x] = 10; // CE OK
    }
}

TEST_CASE("smart-types") {
    map_t<std::string, std::string> map;
    map["hello"] = "world!";
    {
        REQUIRE(map["hello"] == "world!");
        REQUIRE(map.size() == 1);
        REQUIRE(map["abracadabra"] == "");
        REQUIRE(map.size() == 2);
    }

    map.insert({"hello", "new world!"});
    std::pair<std::string, std::string> ss_pair = {"hello", "new world!"};
    map.insert(ss_pair);
    map.insert(std::move(ss_pair));
    {
        REQUIRE(map.size() == 2);
        // insert не изменяет value, если там такой уже был
        // он может только добавить новый
        REQUIRE(map["hello"] == "world!");
        REQUIRE(map["abracadabra"] == "");
    }

    REQUIRE(map.erase("hello") == true);
    {
        REQUIRE(map.size() == 1);
        REQUIRE(map.find("hello") == map.end());
        REQUIRE(map["abracadabra"] == "");
        REQUIRE(map.size() == 1);
    }

    REQUIRE(map.erase("lolasodkaoskdjiawhufa") == false);
    {
        REQUIRE(map.size() == 1);
        REQUIRE(map.find("abracadabra") != map.end());
        REQUIRE(map.find("abracadabra")->second == "");
    }

    map.insert({"bla-bla", "123"});
    map.insert({"C++", "17"});
    map.insert({"C++", "20"});
    {
        // "abracadabra" -> ""
        // "bla-bla" -> "123"
        // "C++" -> "17"
        REQUIRE(map.size() == 3);

        REQUIRE(map.find("abracadabra") != map.end());
        REQUIRE(map.find("abracadabra")->second == "");

        REQUIRE(map.find("bla-bla") != map.end());
        REQUIRE(map.find("bla-bla")->second == "123");

        REQUIRE(map.find("C++") != map.end());
        REQUIRE(map.find("C++")->second == "17");
    }

    map.clear();
    {
        REQUIRE(map.size() == 0);
        REQUIRE(map.empty());
        REQUIRE(map.begin() == map.end());
    }
}

TEST_CASE("permute stress") {
    hashmap<int, std::string> my_map;
    std::unordered_map<int, std::string> stl_map;

    auto verify = [&]() {
        REQUIRE(my_map.size() == stl_map.size());
        REQUIRE(my_map.empty() == stl_map.empty());
        for (auto [x, str] : stl_map) {
            REQUIRE(my_map.find(x) != my_map.end());
            REQUIRE(my_map[x] == str);
        }
    };

    std::vector<int> permut(8);
    iota(permut.begin(), permut.end(), 0);

    std::mt19937 gen(42);

    do {
        std::string str;
        for (int x : permut) {
            str += gen() % ('z' - 'a' + 1) + 'a';

            my_map[x] = str;
            stl_map[x] = str;

            REQUIRE(my_map.find(x) != my_map.end());

            verify();
        }

    } while (next_permutation(permut.begin(), permut.end()));
}

TEST_CASE("small test") {
    hashmap<int, std::string> my_map;
    my_map[0] = "hello";
    my_map[1] = "world";
    my_map[2] = "!";
    my_map[0 + 8] = "kek";
    my_map.erase(0);

    REQUIRE(my_map[0 + 8] == "kek");
    REQUIRE(my_map.bucket(0 + 8) == 0);
    REQUIRE(my_map.size() == 3);
}

TEST_CASE("const") {
    std::vector<std::pair<int, int>> orig_data{{1, 5}, {3, 4}, {2, 1}};
    const hashmap<int, int> map{{1, 5}, {3, 4}, {2, 1}};
    REQUIRE(!map.empty());
    REQUIRE(map.size() == 3);
    auto it = map.find(3);
    REQUIRE(it->second == 4);
    it = map.find(7);
    REQUIRE(it == map.end());
    std::sort(orig_data.begin(), orig_data.end());
    std::vector<std::pair<int, int>> new_data;
    for (const auto &elem : map) {
        new_data.push_back(elem);
    }
    std::sort(new_data.begin(), new_data.end());
    REQUIRE(std::equal(
        orig_data.begin(), orig_data.end(), new_data.begin(), new_data.end()
    ));
}

TEST_CASE("references") {
    std::list<std::pair<int, int>> l{{3, 4}, {8, 5}, {4, 7}, {-1, -3}};
    hashmap<int, int> map(l.begin(), l.end());
    map[3] = 7;
    REQUIRE(map[3] == 7);
    REQUIRE(map.size() == 4);
    REQUIRE(map[0] == 0);
    REQUIRE(map.size() == 5);
}

size_t smart_hash(int) {
    return 0;
}

struct Int {
    int x;

    bool operator==(Int rhs) const {
        return x == rhs.x;
    }
};

struct Hasher {
    size_t operator()(Int x) const {
        return x.x % 17239;
    }
};

TEST_CASE("hash") {
    {
        hashmap<Int, std::string, Hasher> m;
        REQUIRE(m.empty());
        m.insert(Int{0}, "a");
        REQUIRE(m[Int{0}] == "a");
        m.insert(Int{17239}, "b");
        REQUIRE(m[Int{17239}] == "b");
        REQUIRE(m.size() == 2);
        REQUIRE(m.hash_function()(Int{17239}) == 0);
    }
    {
        hashmap<int, int, std::function<size_t(int)>> smart_map(smart_hash);
        auto hash_func = smart_map.hash_function();
        for (int i = 0; i < 1000; ++i) {
            smart_map[i] = i + 1;
            REQUIRE(hash_func(i) == 0);
        }
        REQUIRE(smart_map.size() == 1000);
    }
}

TEST_CASE("iterators") {
    hashmap<int, int> first(10000);
    first.insert(1, 2);
    hashmap<int, int>::iterator it = first.begin();
    REQUIRE(it++ == first.begin());
    REQUIRE(!(it != first.end()));
    REQUIRE(++first.begin() == first.end());
    REQUIRE(*first.begin() == std::make_pair(1, 2));
    REQUIRE(first.begin()->second == 2);
    first.erase(1);
    REQUIRE(first.begin() == first.end());
}

TEST_CASE("backshift") {
    auto h = [](int v) { return v % 1000; };
    hashmap<int, int, decltype(h)> m(1000, h);
    for (int i = 1; i <= 100; ++i) {
        m.insert(i, 100 - i);
        REQUIRE(m.bucket(i) == i);
    }
    REQUIRE(m.size() == 100);
    for (int i = 90; i <= 100; ++i) {
        m[i + 1000] = 1;
    }
    REQUIRE(m.size() == 111);
    REQUIRE(m.bucket(1100) == 111);
    m.erase(50);
    REQUIRE(m.bucket(55) == 55);
    m.erase(1091);
    REQUIRE(m.bucket(1100) == 110);
}

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