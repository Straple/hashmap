#include <iostream>
#include "test_utils.hpp"

TEST_CASE("load factor") {
    map_t m;

    REQUIRE(m.load_factor() == 0);

    for (int i = 0; i < 1'000'000; i++) {
        m[i];
        REQUIRE(m.load_factor() > 0);
        REQUIRE(m.load_factor() <= 0.5);
    }
}