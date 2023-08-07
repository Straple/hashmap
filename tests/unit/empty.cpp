#include "test_utils.hpp"

TEST_CASE("empty object") {
    map_t a;

    REQUIRE(a.find(0) == a.end());
    REQUIRE(a.begin() == a.end());

    a[0];
    REQUIRE(a.begin() != a.end());
    REQUIRE(a.find(1) == a.end());
    REQUIRE(a.find(0) != a.end());
    REQUIRE(a.find(0) != a.find(1));

    a.clear();
    REQUIRE(a.begin() == a.end());
    REQUIRE(a.find(0) == a.end());
    REQUIRE(a.find(1) == a.end());
}