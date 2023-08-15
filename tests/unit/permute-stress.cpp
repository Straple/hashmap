#include "test_utils.hpp"

const int N = 6;

TEST_CASE("permute stress") {
    hashmap<int, std::string> my_map;
    std::unordered_map<int, std::string> stl_map;

    auto verify = [&]() {
        REQUIRE(my_map.size() == stl_map.size());
        REQUIRE(my_map.empty() == stl_map.empty());
        for (auto [x, str] : stl_map) {
            REQUIRE(my_map.find(x) != my_map.end());
            REQUIRE(my_map[x] == str);
        }
    };

    std::vector<int> permut(N);
    iota(permut.begin(), permut.end(), 0);

    std::mt19937 gen(42);

    do {
        std::string str;
        for (int x : permut) {
            str += gen() % ('z' - 'a' + 1) + 'a';

            my_map[x] = str;
            stl_map[x] = str;

            REQUIRE(my_map.find(x) != my_map.end());

            verify();
        }

    } while (next_permutation(permut.begin(), permut.end()));
}