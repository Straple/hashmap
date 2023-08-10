#include "test_utils.hpp"

TEST_CASE("count") {
    static_assert(std::is_same_v<
                  std::size_t,
                  decltype(static_cast<const map_t<>>(map_t<>{}).count(1))>
                  );

    map_t<> a;
    REQUIRE(a.count(1) == 0);
    REQUIRE(a.count(2) == 0);
    a[1] = 0;
    REQUIRE(a.count(1) == 1);
    REQUIRE(a.count(2) == 0);
    a[2] = 0;
    REQUIRE(a.count(1) == 1);
    REQUIRE(a.count(2) == 1);
    a.erase(1);
    REQUIRE(a.count(1) == 0);
    REQUIRE(a.count(2) == 1);
    a.erase(2);
    REQUIRE(a.count(1) == 0);
    REQUIRE(a.count(2) == 0);
}