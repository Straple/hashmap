#include "test_utils.hpp"

TEST_CASE("emplace") {
    map_t<std::string, std::string> m;

    m.emplace(std::make_pair(std::string("a"), std::string("a")));

    m.emplace(std::make_pair("b", "abcd"));

    m.emplace("d", "ddd");

    // recurring keys
    m.emplace("a", "no effect for map");
    m.emplace(std::make_pair("d", "no effect for map"));

    m.emplace(
        std::piecewise_construct, std::forward_as_tuple("c"),
        std::forward_as_tuple(10, 'c')
    );

    // a -> a
    // b -> abcd
    // d -> ddd
    // c -> cccccccccc

    REQUIRE(m.size() == 4);
    REQUIRE(m.find("a")->second == "a");
    REQUIRE(m.find("b")->second == "abcd");
    REQUIRE(m.find("d")->second == "ddd");
    REQUIRE(m.find("c")->second == "cccccccccc");
}

TEST_CASE("emplace-counter"){
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

    SECTION("emplace 1"){
        // 1 move ctor чтобы переместить counter::object в std::pair
        // 1 move ctor, чтобы собрать итоговый объект внутри структуры
        // тут не получиться сразу собрать объект в нужно месте, потому что для этого
        // мы должны знать его хеш, а мы не можем
        m.emplace(counter::object(1, counts), 10);
        REQUIRE(
            counts.data == build_counter_data(
                               {{"ctor", 1},
                                {"copy_ctor", 0},
                                {"move_ctor", 2},
                                {"dtor", 2},
                                {"copy_assign", 0},
                                {"move_assign", 0},
                                {"equals_compare", 0},
                                {"hash_calc", 1}}
                           )
        );
    }

    SECTION("emplace 2"){
        // аналогично "emplace 1"
        m.emplace(counter::object(1, counts), 10);

        REQUIRE(
            counts.data == build_counter_data(
                               {{"ctor", 1},
                                {"copy_ctor", 0},
                                {"move_ctor", 2},
                                {"dtor", 2},
                                {"copy_assign", 0},
                                {"move_assign", 0},
                                {"equals_compare", 0},
                                {"hash_calc", 1}}
                           )
        );
    }

    SECTION("emplace 3"){
        auto val = std::make_pair(counter::object(1, counts), 10);
        REQUIRE(
            counts.data == build_counter_data(
                               {{"ctor", 1},
                                {"copy_ctor", 0},
                                {"move_ctor", 1},
                                {"dtor", 1},
                                {"copy_assign", 0},
                                {"move_assign", 0},
                                {"equals_compare", 0},
                                {"hash_calc", 0}}
                           )
        );
        m.emplace(val); // 1 copy ctor, 1 move ctor, 1 dtor

        REQUIRE(
            counts.data == build_counter_data(
                               {{"ctor", 1},
                                {"copy_ctor", 1},
                                {"move_ctor", 2},
                                {"dtor", 2},
                                {"copy_assign", 0},
                                {"move_assign", 0},
                                {"equals_compare", 0},
                                {"hash_calc", 1}}
                           )
        );
    }

    SECTION("insert 4"){
        std::pair<counter::object, int> val(counter::object(1, counts), 10);
        REQUIRE(
            counts.data == build_counter_data(
                               {{"ctor", 1},
                                {"copy_ctor", 0},
                                {"move_ctor", 1},
                                {"dtor", 1},
                                {"copy_assign", 0},
                                {"move_assign", 0},
                                {"equals_compare", 0},
                                {"hash_calc", 0}}
                           )
        );
        m.insert(std::move(val)); // 1 move

        REQUIRE(
            counts.data == build_counter_data(
                               {{"ctor", 1},
                                {"copy_ctor", 0},
                                {"move_ctor", 2},
                                {"dtor", 1},
                                {"copy_assign", 0},
                                {"move_assign", 0},
                                {"equals_compare", 0},
                                {"hash_calc", 1}}
                           )
        );
    }
}