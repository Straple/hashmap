#include "test_utils.hpp"

TEST_CASE("erase") {
    map_t<> a;

    a.insert({123, 0});
    a.insert({456, 0});
    REQUIRE(a.erase(123) == 1);
    REQUIRE(a.erase(456) == 1);
    REQUIRE(a.empty());
    REQUIRE(a.find(123) == a.end());
    REQUIRE(a.find(456) == a.end());
}

// validate correctness backshift algorithm in erase
TEST_CASE("backshift") {
    auto h = [](int v) { return v % 1000; };
    map_t<int, int, decltype(h)> m(1000, h);
    for (int i = 1; i <= 100; ++i) {
        m.emplace(i, 100 - i);
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