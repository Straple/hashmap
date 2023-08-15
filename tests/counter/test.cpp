#include <catch.hpp>
#include <iostream>
#include "counter.hpp"

TEST_CASE("build_counter_data") {
    REQUIRE_THROWS_AS(build_counter_data({{"hello o_O", 0}}), std::string);

    auto data =
        build_counter_data({{"ctor", 5}, {"hash_calc", 1}, {"dtor", 9}});
    REQUIRE(data.m_ctor == 5);
    REQUIRE(data.m_copy_ctor == 0);
    REQUIRE(data.m_move_ctor == 0);
    REQUIRE(data.m_dtor == 9);
    REQUIRE(data.m_copy_assign == 0);
    REQUIRE(data.m_move_assign == 0);
    REQUIRE(data.m_equals_compare == 0);
    REQUIRE(data.m_hash_calc == 1);
}

TEST_CASE("counter one object") {
    counter counts;
    {
        counter::object x(0, counts);
        REQUIRE(
            counts.data == build_counter_data({
                               {"ctor", 1},
                               {"copy_ctor", 0},
                               {"move_ctor", 0},
                               {"dtor", 0},
                               {"copy_assign", 0},
                               {"move_assign", 0},
                               {"equals_compare", 0},
                               {"hash_calc", 0},
                           })
        );
        x == x;
        REQUIRE(
            counts.data == build_counter_data({
                               {"ctor", 1},
                               {"copy_ctor", 0},
                               {"move_ctor", 0},
                               {"dtor", 0},
                               {"copy_assign", 0},
                               {"move_assign", 0},
                               {"equals_compare", 1},
                               {"hash_calc", 0},
                           })
        );
        std::hash<decltype(x)>{}(x);
        REQUIRE(
            counts.data == build_counter_data({
                               {"ctor", 1},
                               {"copy_ctor", 0},
                               {"move_ctor", 0},
                               {"dtor", 0},
                               {"copy_assign", 0},
                               {"move_assign", 0},
                               {"equals_compare", 1},
                               {"hash_calc", 1},
                           })
        );

        x = std::move(x);
        REQUIRE(
            counts.data == build_counter_data({
                               {"ctor", 1},
                               {"copy_ctor", 0},
                               {"move_ctor", 0},
                               {"dtor", 0},
                               {"copy_assign", 0},
                               {"move_assign", 1},
                               {"equals_compare", 1},
                               {"hash_calc", 1},
                           })
        );
    }
    REQUIRE(
        counts.data == build_counter_data({
                           {"ctor", 1},
                           {"copy_ctor", 0},
                           {"move_ctor", 0},
                           {"dtor", 1},
                           {"copy_assign", 0},
                           {"move_assign", 1},
                           {"equals_compare", 1},
                           {"hash_calc", 1},
                       })
    );
}

TEST_CASE("counter objects in vector allocate") {
    counter counts;
    {
        // создается один элемент, а затем значение копируется в 10 элементов
        // вектора и после удаляется этот элемент
        std::vector<counter::object> v(10, {0, counts});
        REQUIRE(
            counts.data == build_counter_data({
                               {"ctor", 1},
                               {"copy_ctor", 10},
                               {"move_ctor", 0},
                               {"dtor", 1},
                               {"copy_assign", 0},
                               {"move_assign", 0},
                               {"equals_compare", 0},
                               {"hash_calc", 0},
                           })
        );
    }  // удаляются 10 элементов
    REQUIRE(
        counts.data == build_counter_data({
                           {"ctor", 1},
                           {"copy_ctor", 10},
                           {"move_ctor", 0},
                           {"dtor", 1 + 10},
                           {"copy_assign", 0},
                           {"move_assign", 0},
                           {"equals_compare", 0},
                           {"hash_calc", 0},
                       })
    );
}

TEST_CASE("counter objects in vector push_back") {
    counter counts;
    {
        std::vector<counter::object> v;
        REQUIRE(
            counts.data == build_counter_data({
                               {"ctor", 0},
                               {"copy_ctor", 0},
                               {"move_ctor", 0},
                               {"dtor", 0},
                               {"copy_assign", 0},
                               {"move_assign", 0},
                               {"equals_compare", 0},
                               {"hash_calc", 0},
                           })
        );

        for (int i = 0; i < 64; i++) {
            v.push_back({1, counts});
        }

        // std::cout << counts.data << '\n';

        REQUIRE(
            counts.data == build_counter_data(
                               {{"ctor", 64},
                                {"copy_ctor", 63},
                                {"move_ctor", 64},
                                {"dtor", 127},
                                {"copy_assign", 0},
                                {"move_assign", 0},
                                {"equals_compare", 0},
                                {"hash_calc", 0}}
                           )
        );
    }
    REQUIRE(
        counts.data == build_counter_data(
                           {{"ctor", 64},
                            {"copy_ctor", 63},
                            {"move_ctor", 64},
                            {"dtor", 127 + 64},
                            {"copy_assign", 0},
                            {"move_assign", 0},
                            {"equals_compare", 0},
                            {"hash_calc", 0}}
                       )
    );
}
