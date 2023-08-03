#include "test_utils.hpp"

TEST_CASE("assign to moved") {
    map_t a;
    a[1] = 2;
    map_t moved = std::move(a);
    REQUIRE(moved.size() == 1);
    REQUIRE(a.size() == 0);
    REQUIRE(a.empty());

    {
        map_t c;
        c[3] = 4;
        c[2] = 4;

        a = c;
        REQUIRE(c.size() == 2);
    }
    REQUIRE(a.size() == 2);
    REQUIRE(!a.empty());
}

TEST_CASE("moved_to_moved") {
    map_t a;
    a[1] = 2;
    map_t moved = std::move(a);

    map_t c;
    c[3] = 4;

    a = std::move(c);

    a[5] = 6;
    moved[6] = 7;
    REQUIRE(a[5] == 6);
    REQUIRE(a[3] == 4);
    REQUIRE(moved[6] == 7);
}

TEST_CASE("swap") {
    {
        map_t a;
        a[1] = 2;
        {
            map_t b;
            std::swap(a, b);
            REQUIRE(a.find(1) == a.end());
            REQUIRE(b.find(1) != b.end());
            REQUIRE(b.find(1)->second == 2);
        }
        REQUIRE(a.find(1) == a.end());
        a[2] = 3;
        REQUIRE(a.find(2) != a.end());
        REQUIRE(a.size() == 1);
    }

    {
        map_t a;
        {
            map_t b;
            b[1] = 2;
            std::swap(a, b);
            REQUIRE(a.find(1) != a.end());
            REQUIRE(a.find(1)->second == 2);
            REQUIRE(b.find(1) == b.end());
        }
        REQUIRE(a.find(1) != a.end());
        a[2] = 3;
        REQUIRE(a.size() == 2);
        REQUIRE(a.find(2) != a.end());
        REQUIRE(a.find(2) != a.find(1));
    }

    {
        map_t a;
        {
            map_t b;
            std::swap(a, b);
            REQUIRE(a.end() == a.find(1));
            REQUIRE(b.end() == b.find(1));
        }
        REQUIRE(a.end() == a.find(1));
    }
}