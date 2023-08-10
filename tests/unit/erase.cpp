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