#include <memory>
#include "test_utils.hpp"

TEST_CASE("unique_ptr") {
    hashmap<int, std::unique_ptr<int>> m;
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
    hashmap<int, std::unique_ptr<int>> m;
    for (int i = 0; i < 1000; i++) {
        m.insert(i, new int(i));
        m.insert(i, new int(i));  // should no memory leak
    }
    {
        hashmap<std::unique_ptr<int>, int> m;
        for (int i = 0; i < 1000; i++) {
            m.insert(new int(i), i);
            m.insert(new int(i), i);
        }
    }
    {
        hashmap<std::unique_ptr<int>, int> m;
        for (int i = 0; i < 1000; i++) {
            std::unique_ptr<int> p(new int(i));
            m.insert(std::move(p), i);
            m.insert(std::move(p), i);
        }
    }
}

TEST_CASE("value is unique_ptr") {
    hashmap<int, std::unique_ptr<int>> m;
    m.insert(0, std::make_unique<int>(0));
    m[1] = std::make_unique<int>(1);
    std::unique_ptr<int> p(new int(10));
    m[2] = std::move(p);
    p = std::make_unique<int>(20);
    m.insert(3, std::move(p));
}

TEST_CASE("key is unique_ptr") {
    hashmap<std::unique_ptr<int>, int> m;
    m.insert(new int(10), 0);
    m.insert(new int(20), 0);
    m[new int(30)] = 0;
    std::unique_ptr<int> p(new int(40));
    m.insert(std::move(p), 0);
    m[std::move(p)] = 0;
}