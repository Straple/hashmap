#include "test_utils.hpp"

struct noncopyable {
    int x = 0;

    noncopyable(int x) : x(x) {
    }

    noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

    noncopyable(noncopyable &&) = default;
    noncopyable &operator=(noncopyable &&) = default;

    friend bool operator==(const noncopyable &lhs, const noncopyable &rhs) {
        return lhs.x == rhs.x;
    }
};

TEST_CASE("noncopyable value") {
    hashmap<int, noncopyable> map;
    map.insert({10, noncopyable(5)});
    noncopyable x(5);
    map.insert({10, std::move(x)});
    map.insert(std::make_pair(10, noncopyable(5)));
    map.insert(std::make_pair(10, std::move(x)));

    map.erase(10);

    map[5] = std::move(x);
    map[5] = 10;
}

TEST_CASE("noncopyable key") {
    struct hasher {
        std::size_t operator()(const noncopyable &x) const {
            return x.x;
        }
    };

    hashmap<noncopyable, int, hasher> map;
    map.insert({noncopyable(5), 10});
    noncopyable x(5);
    map.insert({std::move(x), 10});

    map.insert(std::make_pair(noncopyable(5), 10));
    map.insert(std::make_pair(std::move(x), 10));

    map.erase(5);
    map.erase(x);
    map.erase(std::move(x));

    map[noncopyable(5)] = 10;
    map[std::move(x)] = 10;

    // map[x] = 10; // CE OK
}