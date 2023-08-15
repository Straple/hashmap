#include "test_utils.hpp"

TEST_CASE("insert trivial") {
    map_t<> a;

    a.insert({123, 456});
    REQUIRE(a.size() == 1);
    REQUIRE(a.find(123) == a.begin());
    REQUIRE(a.find(123) != a.end());
    REQUIRE(a.find(123)->second == 456);

    a.insert({0, 4});
    REQUIRE(a.size() == 2);
    REQUIRE(a.find(123) != a.end());
    REQUIRE(a.find(123)->second == 456);
    REQUIRE(a.find(0) != a.end());
    REQUIRE(a.find(0)->second == 4);
}

TEST_CASE("insert smart") {
    map_t<int, std::string> m = {{1, "one"}, {2, "two"}};
    m.insert({3, "three"});
    m.insert(std::make_pair(4, "four"));
    m.insert({{4, "another four"}, {5, "five"}});

    REQUIRE(!m.insert({1, "another one"}).second);

    // 1 -> one
    // 2 -> two
    // 3 -> three
    // 4 -> four
    // 5 -> five

    REQUIRE(m.size() == 5);
    REQUIRE(m.find(1)->second == "one");
    REQUIRE(m.find(2)->second == "two");
    REQUIRE(m.find(3)->second == "three");
    REQUIRE(m.find(4)->second == "four");
    REQUIRE(m.find(5)->second == "five");
}

TEST_CASE("insert-counter"){
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

    SECTION("insert 1"){
        // 1 move ctor чтобы переместить counter::object в std::pair
        // 1 move ctor, чтобы собрать итоговый объект внутри структуры
        m.insert(std::make_pair(counter::object(1, counts), 10));
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

    SECTION("insert 2"){
        // 1 move ctor чтобы переместить counter::object в std::pair
        // 1 move ctor, чтобы собрать итоговый объект внутри структуры
        m.insert({counter::object(1, counts), 10});

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

    SECTION("insert 3"){
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
        m.insert(val); // 1 copy

        REQUIRE(
            counts.data == build_counter_data(
                               {{"ctor", 1},
                                {"copy_ctor", 1},
                                {"move_ctor", 1},
                                {"dtor", 1},
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