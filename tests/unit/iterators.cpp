#include "test_utils.hpp"

// begin() при разных const квалификаторах выдает разные итераторы
static_assert(std::is_same_v<decltype(map_t<>().begin()), map_t<>::iterator>);
static_assert(std::is_same_v<
              decltype(static_cast<const map_t<>>(map_t<>()).begin()),
              map_t<>::const_iterator>);

// аналогично end()
static_assert(std::is_same_v<decltype(map_t<>().end()), map_t<>::iterator>);
static_assert(std::is_same_v<
              decltype(static_cast<const map_t<>>(map_t<>()).end()),
              map_t<>::const_iterator>);

// аналогично find()
static_assert(std::is_same_v<decltype(map_t<>().find(0)), map_t<>::iterator>);
static_assert(std::is_same_v<
              decltype(static_cast<const map_t<>>(map_t<>()).find(0)),
              map_t<>::const_iterator>);

// cbegin(), cend() возвращают const_iterator
static_assert(std::is_same_v<
              decltype(static_cast<const map_t<>>(map_t<>()).cbegin()),
              map_t<>::const_iterator>);
static_assert(std::is_same_v<
              decltype(static_cast<const map_t<>>(map_t<>()).cend()),
              map_t<>::const_iterator>);

TEST_CASE("const_iterator") {
    std::vector<std::pair<int, int>> data{{1, 5}, {3, 4}, {2, 1}};
    const map_t<> map{{1, 5}, {3, 4}, {2, 1}};
    REQUIRE(!map.empty());
    REQUIRE(map.size() == 3);
    auto it = map.find(3);
    REQUIRE(it->second == 4);
    it = map.find(7);  // may copy iterator
    REQUIRE(it == map.end());

    // validate data
    {
        std::vector<std::pair<int, int>> map_data;
        // range based for works
        for (const auto &elem : map) {
            map_data.push_back(elem);
        }

        std::sort(data.begin(), data.end());
        std::sort(map_data.begin(), map_data.end());

        REQUIRE(data == map_data);
    }
}

TEST_CASE("iterator"){
    map_t<> m(10000);
    m.emplace(1, 2);
    auto it = m.begin();
    REQUIRE(it++ == m.begin());
    REQUIRE(it == m.end());
    REQUIRE(!(it != m.end()));
    REQUIRE(++m.begin() == m.end());
    REQUIRE(*m.begin() == std::make_pair(1, 2));
    REQUIRE(m.begin()->second == 2);
    m.erase(1);
    REQUIRE(m.begin() == m.end());
}
