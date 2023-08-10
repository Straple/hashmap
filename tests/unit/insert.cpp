#include "test_utils.hpp"

TEST_CASE("insert") {
    map_t<> a;

    a.insert({123, 456});
    REQUIRE(a.size() == 1);
    REQUIRE(a.find(123) == a.begin());
    REQUIRE(a.find(123) != a.end());
    REQUIRE(a.find(123)->second == 456);

    a.insert({0, 4});
    REQUIRE(a.size() == 2);
    REQUIRE(a.find(123) != a.end());
    REQUIRE(a.find(123)->second == 456);
    REQUIRE(a.find(0) != a.end());
    REQUIRE(a.find(0)->second == 4);
}