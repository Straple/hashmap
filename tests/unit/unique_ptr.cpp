#include <memory>
#include "test_utils.hpp"

TEST_CASE("unique_ptr") {
    map_t<int, std::unique_ptr<int>> m;
    m[0] = std::make_unique<int>(123);
    REQUIRE(m.begin() != m.end());
    REQUIRE(m.find(1) == m.end());
    REQUIRE(m.find(0)->second);

    for (const auto &kv : m) {
        REQUIRE(kv.first == 0);
        REQUIRE(kv.second != nullptr);
        REQUIRE(*kv.second == 123);
    }

    m.clear();

    m[0];
    REQUIRE(!m.find(0)->second);
    m[0] = std::make_unique<int>(10);
    REQUIRE(m.find(0)->second);
    REQUIRE(*m.find(0)->second == 10);
}

TEST_CASE("unique_ptr fill") {
    {
        map_t<int, std::unique_ptr<int>> m;
        for (int i = 0; i < 1000; i++) {
            m.emplace(i, new int(i));
            m.emplace(i, new int(i));  // should no memory leak
        }
    }
    {
        map_t<std::unique_ptr<int>, int> m;
        for (int i = 0; i < 1000; i++) {
            m.emplace(new int(i), i);
            m.emplace(new int(i), i);
        }
    }
    {
        map_t<std::unique_ptr<int>, int> m;
        for (int i = 0; i < 1000; i++) {
            std::unique_ptr<int> p(new int(i));
            m.emplace(std::move(p), i);
            m.emplace(std::move(p), i);
        }
    }
}

TEST_CASE("value is unique_ptr") {
    map_t<int, std::unique_ptr<int>> m;
    m.emplace(0, std::make_unique<int>(0));
    m[1] = std::make_unique<int>(1);
    std::unique_ptr<int> p(new int(10));
    m[2] = std::move(p);
    p = std::make_unique<int>(20);
    m.emplace(3, std::move(p));
}

TEST_CASE("key is unique_ptr") {
    map_t<std::unique_ptr<int>, int> m;
    m.emplace(new int(10), 0);
    m.emplace(new int(20), 0);
    // m[new int(30)] = 0;
    // std::unique_ptr<int> p(new int(40));
    // m.emplace(std::move(p), 0);
    // m[std::move(p)] = 0;
}