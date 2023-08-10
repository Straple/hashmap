#include "test_utils.hpp"

TEST_CASE("contains") {
    static_assert(std::is_same_v<
                  bool,
                  decltype(static_cast<const map_t<>>(map_t<>{}).contains(1))>);

    map_t<> a;
    REQUIRE(!a.contains(1));
    REQUIRE(!a.contains(2));
    a[1] = 0;
    REQUIRE(a.contains(1));
    REQUIRE(!a.contains(2));
    a[2] = 0;
    REQUIRE(a.contains(1));
    REQUIRE(a.contains(2));
    a.erase(1);
    REQUIRE(!a.contains(1));
    REQUIRE(a.contains(2));
    a.erase(2);
    REQUIRE(!a.contains(1));
    REQUIRE(!a.contains(2));
}