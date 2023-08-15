#include "../counter/counter.hpp"
#include "test_utils.hpp"
#include <list>

// TODO: доделать

TEST_CASE("ctors") {
    counter counts;

    map_t<counter::object, int> m;

    REQUIRE(
        counts.data == build_counter_data(
                           {{"ctor", 0},
                            {"copy_ctor", 0},
                            {"move_ctor", 0},
                            {"dtor", 0},
                            {"copy_assign", 0},
                            {"move_assign", 0},
                            {"equals_compare", 0},
                            {"hash_calc", 0}}
                       )
    );
}

TEST_CASE("ctor from iterators begin, end"){
    std::list<std::pair<int, int>> l{{3, 4}, {8, 5}, {4, 7}, {-1, -3}};
    map_t<> map(l.begin(), l.end());
    map[3] = 7;
    REQUIRE(map[3] == 7);
    REQUIRE(map.size() == 4);
    REQUIRE(map[0] == 0);
    REQUIRE(map.size() == 5);
}