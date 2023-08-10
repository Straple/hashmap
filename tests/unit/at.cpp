#include "test_utils.hpp"

TEST_CASE("at") {
    map_t<> a;

    REQUIRE_THROWS_AS(a.at(0), std::out_of_range);

    const map_t<> &c = a;

    REQUIRE_THROWS_AS(c.at(0), std::out_of_range);

    a[0] = 1;
    REQUIRE(a.at(0) == 1);
    REQUIRE(c.at(0) == 1);

    REQUIRE_THROWS_AS(a.at(1), std::out_of_range);
    REQUIRE_THROWS_AS(c.at(1), std::out_of_range);
}