#include "test_utils.hpp"

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

size_t hash_function(int) {
    return 0;
}

TEST_CASE("hasher") {
    {
        hashmap<Int, std::string, Hasher> m;
        REQUIRE(m.empty());
        m.emplace(Int{0}, "a");
        REQUIRE(m[Int{0}] == "a");
        m.emplace(Int{17239}, "b");
        REQUIRE(m[Int{17239}] == "b");
        REQUIRE(m.size() == 2);
        REQUIRE(m.hash_function()(Int{17239}) == 0);
    }
    {
        hashmap<int, int, std::function<size_t(int)>> smart_map(hash_function);
        auto hash_func = smart_map.hash_function();
        for (int i = 0; i < 1000; ++i) {
            smart_map[i] = i + 1;
            REQUIRE(hash_func(i) == 0);
        }
        REQUIRE(smart_map.size() == 1000);
    }
}