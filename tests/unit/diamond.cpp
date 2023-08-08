#include "test_utils.hpp"

struct hash_with_equal {
    std::size_t operator()(int x) const{
        return static_cast<std::size_t>(x);
    }

    bool operator()(int a, int b) const{
        return a == b;
    }
};

TEST_CASE("diamond_problem") {
    hashmap<int, int, hash_with_equal> map;
    map[1] = 2;
    REQUIRE(map.size() == 1);
    REQUIRE(map.find(1) != map.end());
}