#include "../counter/counter.hpp"
#include "test_utils.hpp"

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