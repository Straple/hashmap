#include "test_utils.hpp"

TEST_CASE("emplace") {
    map_t<std::string, std::string> m;

    m.emplace(std::make_pair(std::string("a"), std::string("a")));

    m.emplace(std::make_pair("b", "abcd"));

    m.emplace("d", "ddd");

    // recurring keys
    m.emplace("a", "no effect for map");
    m.emplace(std::make_pair("d", "no effect for map"));

    m.emplace(
        std::piecewise_construct, std::forward_as_tuple("c"),
        std::forward_as_tuple(10, 'c')
    );

    // a -> a
    // b -> abcd
    // d -> ddd
    // c -> cccccccccc

    REQUIRE(m.size() == 4);
    REQUIRE(m.find("a")->second == "a");
    REQUIRE(m.find("b")->second == "abcd");
    REQUIRE(m.find("d")->second == "ddd");
    REQUIRE(m.find("c")->second == "cccccccccc");
}