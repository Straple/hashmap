#include "test_utils.hpp"

TEST_CASE("insert trivial") {
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

TEST_CASE("insert smart") {
    map_t<int, std::string> m = {{1, "one"}, {2, "two"}};
    m.insert({3, "three"});
    m.insert(std::make_pair(4, "four"));
    m.insert({{4, "another four"}, {5, "five"}});

    REQUIRE(!m.insert({1, "another one"}).second);

    // 1 -> one
    // 2 -> two
    // 3 -> three
    // 4 -> four
    // 5 -> five

    REQUIRE(m.size() == 5);
    REQUIRE(m.find(1)->second == "one");
    REQUIRE(m.find(2)->second == "two");
    REQUIRE(m.find(3)->second == "three");
    REQUIRE(m.find(4)->second == "four");
    REQUIRE(m.find(5)->second == "five");
}