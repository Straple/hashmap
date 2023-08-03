#include <iostream>
#include "test_utils.hpp"

// all assignments
// object can be:
// 0) default empty (после конструктора по умолчанию)
// 1-2) non empty (в нем есть некоторые элементы). 1 означает только 1 элемент,
// 2 - много элементов
// 2) cleared (после .clear())
// 3) moved from (из него
// замували)

TEST_CASE() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            map_t a;
            if (i == 0) {
            }
        }
    }
}

TEST_CASE("default empty = default empty") {
    map_t a, b;
    SECTION("copy assign") {
        b = a;
    }
    SECTION("move assign") {
        b = std::move(a);
    }
    REQUIRE(a == b);
    REQUIRE(a.empty());
    REQUIRE(b.empty());
}

TEST_CASE("default empty = non empty") {
    map_t a, b;
    SECTION("b is non empty, small") {
        b[1] = 2;
    }
    SECTION("b is non empty, big") {
        for (int i = 0; i < 1000; i++) {
            b[i] = -1;
        }
    }

    a = b;
    REQUIRE(a == b);
}

TEST_CASE("default empty = cleared") {
    map_t a, b;
    SECTION("b is default empty") {
    }
    SECTION("b is non empty, small") {
        b[1] = 2;
    }
    SECTION("b is non empty, big") {
        for (int i = 0; i < 1000; i++) {
            b[i] = -1;
        }
    }

    b.clear();
    a = b;
    REQUIRE(a == b);
    REQUIRE(a.empty());
    REQUIRE(b.empty());
}

TEST_CASE("default empty = moved from") {
    map_t a, b;
    SECTION("b is default empty") {
    }
    SECTION("b is non empty, small") {
        b[1] = 2;
    }
    SECTION("b is non empty, big") {
        for (int i = 0; i < 1000; i++) {
            b[i] = -1;
        }
    }

    map_t moved = std::move(b);
    a = b;
    REQUIRE(a == b);
    REQUIRE(a.empty());
    REQUIRE(b.empty());
}

TEST_CASE("changing a does not change b") {
    map_t a;
    const map_t &a_const = a;
    map_t b;

    a[123] = 228;
    REQUIRE(a != b);
    b = a;
    REQUIRE(a == b);

    REQUIRE(a.find(123)->second == 228);
    REQUIRE(a_const.find(123)->second == 228);
    REQUIRE(b.find(123)->second == 228);

    a[123] = 42;
    REQUIRE(a.find(123)->second == 42);
    REQUIRE(a_const.find(123)->second == 42);
    REQUIRE(b.find(123)->second == 228);

    b[123] = -1;
    REQUIRE(a.find(123)->second == 42);
    REQUIRE(a_const.find(123)->second == 42);
    REQUIRE(b.find(123)->second == -1);
}

TEST_CASE("copy assign") {
    map_t a, b;
    SECTION("small") {
        b[123] = 228;
    }
    SECTION("big") {
        for (int i = 0; i < 1000; i++) {
            b[i] = -1;
        }
    }
    b = a;

    REQUIRE(a.size() == 0);
    REQUIRE(b.size() == 0);
}

TEST_CASE("assignment combinations 5") {
    map_t a, b;
    SECTION("small") {
        b[123] = 228;
    }
    SECTION("big") {
        for (int i = 0; i < 1000; i++) {
            b[i] = -1;
        }
    }

    b.clear();
    b = a;

    REQUIRE(a.size() == 0);
    REQUIRE(b.size() == 0);
}

TEST_CASE("assignment combinations 6") {
}