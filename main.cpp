#include <iostream>
#include <string>
#include "hashmap.h"

enum METHOD {
    DEFAULT_CONSTRUCTOR,
    COPY_CONSTRUCTOR,
    MOVE_CONSTRUCTOR,
    DESTRUCTOR,
    COPY_ASSIGN,
    MOVE_ASSIGN,
};

struct visor_t {
    std::string str;

    visor_t(const std::string &str_) {
        str = str_;
        std::cout << "build constructor: " << str << std::endl;
    }

    visor_t(const char *str_) {
        str = str_;
        std::cout << "build constructor: " << str << std::endl;
    }

    visor_t() {
        str = "";
        std::cout << "default constructor \"" << str << "\"" << std::endl;
    }

    visor_t(const visor_t &other) {
        std::cout << "copy constructor \"" << str << "\" = \"" << other.str
                  << "\"" << std::endl;
        str = other.str;
    }

    visor_t(visor_t &&other) noexcept {
        std::cout << "move constructor \"" << str << "\" = \"" << other.str
                  << "\"" << std::endl;
        str = std::move(other.str);
    }

    ~visor_t() {
        std::cout << "destructor \"" << str << "\"" << std::endl;
    }

    visor_t &operator=(const visor_t &other) {
        std::cout << "copy assign \"" << str << "\" = \"" << other.str << "\""
                  << std::endl;
        str = other.str;
        return *this;
    }

    visor_t &operator=(visor_t &&other) noexcept {
        std::cout << "move assign \"" << str << "\" = \"" << other.str << "\""
                  << std::endl;
        str = std::move(other.str);
        return *this;
    }
};

bool operator==(const visor_t &lhs, const visor_t &rhs) {
    return lhs.str == rhs.str;
}

struct visor_hasher {
    size_t operator()(const visor_t &x) const {
        return std::hash<std::string>{}(x.str);
    }
};

int main() {
    hashmap<visor_t, int, visor_hasher> map;
    map["hello"] = 10;
    map["world"] = 5;
    map.clear();
}