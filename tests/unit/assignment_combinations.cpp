#include <iostream>
#include "test_utils.hpp"

// состояния объекта:
// 0) default empty (после конструктора по умолчанию)
// 1) empty, удалили все элементы, например .clear()
// 2) no empty
// 3) moved from (из него замували)

map_t build_map() {
    map_t map;
    for (int i = 0; i < 1000; i++) {
        map[i] = -1;
    }
    return map;
}

TEST_CASE("copy from default empty") {
    map_t a = build_map();
    map_t b;

    SECTION("copy assign") {
        a = b;
    }
    SECTION("move assign") {
        a = std::move(b);
    }
    REQUIRE(a == b);
    REQUIRE(a.empty());
    REQUIRE(b.empty());
}

TEST_CASE("copy from non empty") {
    map_t a = build_map();
    map_t b;
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
    REQUIRE(!a.empty());
    REQUIRE(!b.empty());
}

TEST_CASE("copy from cleared") {
    map_t a = build_map();
    map_t b;
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

TEST_CASE("copy from moved from") {
    map_t a = build_map();
    map_t b;

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

TEST_CASE("some combinations") {
    map_t a, b;
    a[1] = 2;
    b[3] = 4;
    a = b;

    REQUIRE(a.size() == 1);
    REQUIRE(b.size() == 1);

    REQUIRE(b.find(3)->second == 4);
    REQUIRE(a.find(1) == a.end());

    a[1] = 2;
    REQUIRE(a.size() == 2);
    REQUIRE(b.size() == 1);
    REQUIRE(b.find(1) == b.end());
    REQUIRE(a.find(1)->second == 2);
}

TEST_CASE("self assigment") {
    map_t a;
    a[1] = 2;
    a = a;
    REQUIRE(a == a);
    REQUIRE(a.size() == 1);
    REQUIRE(a.find(1)->second == 2);

    a = std::move(a);
    REQUIRE(a == a);
    REQUIRE(a.empty());
    REQUIRE(a.find(1) == a.end());
}

// TODO: может это убрать в другое место?
TEST_CASE("compare == and !=") {
    map_t a, b;

    auto verify = [&](bool is_eq) {
        REQUIRE((a == b) == is_eq);
        REQUIRE((b == a) == is_eq);
        REQUIRE((!(a != b)) == is_eq);
        REQUIRE((!(b != a)) == is_eq);
    };

    a[1] = 2;
    verify(false);
    b[2] = 1;
    verify(false);
    a[2] = 1;
    verify(false);
    b[1] = 2;
    verify(true);
    a.clear();
    verify(false);
    b.clear();
    verify(true);

    for (int i = 0; i < 1000; i++) {
        a[i] = -1;
        verify(false);
    }
    for (int i = 999; i >= 0; i--) {
        verify(false);
        b[i] = -1;
    }
    verify(true);
}