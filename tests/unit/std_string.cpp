#include "test_utils.hpp"

TEST_CASE("std::string") {
    map_t<std::string, std::string> map;
    map["hello"] = "world!";
    {
        REQUIRE(map["hello"] == "world!");
        REQUIRE(map.size() == 1);
        REQUIRE(map["abracadabra"] == "");
        REQUIRE(map.size() == 2);
    }

    map.insert({"hello", "new world!"});
    std::pair<std::string, std::string> ss_pair = {"hello", "new world!"};
    map.insert(ss_pair);
    map.insert(std::move(ss_pair));
    {
        REQUIRE(map.size() == 2);
        // insert не изменяет value, если там такой уже был
        // он может только добавить новый
        REQUIRE(map["hello"] == "world!");
        REQUIRE(map["abracadabra"] == "");
    }

    REQUIRE(map.erase("hello") == true);
    {
        REQUIRE(map.size() == 1);
        REQUIRE(map.find("hello") == map.end());
        REQUIRE(map["abracadabra"] == "");
        REQUIRE(map.size() == 1);
    }

    REQUIRE(map.erase("lolasodkaoskdjiawhufa") == false);
    {
        REQUIRE(map.size() == 1);
        REQUIRE(map.find("abracadabra") != map.end());
        REQUIRE(map.find("abracadabra")->second == "");
    }

    map.insert({"bla-bla", "123"});
    map.insert({"C++", "17"});
    map.insert({"C++", "20"});
    {
        // "abracadabra" -> ""
        // "bla-bla" -> "123"
        // "C++" -> "17"
        REQUIRE(map.size() == 3);

        REQUIRE(map.find("abracadabra") != map.end());
        REQUIRE(map.find("abracadabra")->second == "");

        REQUIRE(map.find("bla-bla") != map.end());
        REQUIRE(map.find("bla-bla")->second == "123");

        REQUIRE(map.find("C++") != map.end());
        REQUIRE(map.find("C++")->second == "17");
    }

    map.clear();
    {
        REQUIRE(map.size() == 0);
        REQUIRE(map.empty());
        REQUIRE(map.begin() == map.end());
    }
}

TEST_CASE("std::string is value") {
    map_t<int, std::string> my_map;
    my_map[0] = "hello";
    my_map[1] = "world";
    my_map[2] = "!";
    my_map[0 + 8] = "kek";
    my_map.erase(0);

    REQUIRE(my_map[0 + 8] == "kek");
    REQUIRE(my_map.bucket(0 + 8) == 0);  // only for hashmap
    REQUIRE(my_map.size() == 3);
}