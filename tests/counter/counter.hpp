#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>

#define CAT_IMPL(lhs, rhs) lhs##rhs
#define CAT(lhs, rhs) CAT_IMPL(lhs, rhs)

struct counter {
    struct data_t {
        std::size_t m_ctor, m_copy_ctor, m_move_ctor, m_dtor, m_copy_assign,
            m_move_assign, m_equals_compare, m_hash_calc;

        friend bool operator==(const data_t &lhs, const data_t &rhs) {
#define COUNTER_COMPARE(x) CAT(lhs.m_, x) == CAT(rhs.m_, x)

            return COUNTER_COMPARE(ctor) && COUNTER_COMPARE(copy_ctor) &&
                   COUNTER_COMPARE(move_ctor) && COUNTER_COMPARE(dtor) &&
                   COUNTER_COMPARE(copy_assign) &&
                   COUNTER_COMPARE(move_assign) &&
                   COUNTER_COMPARE(equals_compare) &&
                   COUNTER_COMPARE(hash_calc);

#undef COUNTER_COMPARE
        }

    } data = {};

    struct object {
        object() : x(0), m_counts(nullptr) {
        }

        object(int x, counter &counts) : x(x), m_counts(&counts) {
            m_counts->data.m_ctor++;
        }

        object(const object &other) : x(other.x), m_counts(other.m_counts) {
            if (m_counts) {
                m_counts->data.m_copy_ctor++;
            }
        }

        object(object &&other) : x(other.x), m_counts(other.m_counts) {
            if (m_counts) {
                m_counts->data.m_move_ctor++;
            }
        }

        ~object() {
            if (m_counts) {
                m_counts->data.m_dtor++;
            }
        }

        object &operator=(const object &other) {
            m_counts = other.m_counts;
            if (m_counts) {
                m_counts->data.m_copy_assign++;
            }
            x = other.x;
        }

        object &operator=(object &&other) {
            m_counts = other.m_counts;
            if (m_counts) {
                m_counts->data.m_move_assign++;
            }
            x = other.x;
        }

        friend bool operator==(const object &lhs, const object &rhs) {
            if (lhs.m_counts) {
                lhs.m_counts->data.m_equals_compare++;
            }
            return lhs.x == rhs.x;
        }

        std::size_t get_hash() const {
            if (nullptr != m_counts) {
                ++m_counts->data.m_hash_calc;
            }
            return std::hash<int>{}(x);
        }

        counter *m_counts;
        int x;  // some value
    };
};

// example:
// build_counter_data({
//    {"ctor", 9},
//    {"copy_ctor", 0},
//    {"move_ctor", 5},
//    {"dtor", 7},
//    {"copy_assign", 1},
//    {"move_assign", 0},
//    {"equals_compare", 3},
//    {"hash_calc", 0},
// });
inline counter::data_t build_counter_data(
    std::initializer_list<std::pair<std::string, std::size_t>> list
) {
    counter::data_t data = {};
    for (auto [name, value] : list) {
#define COUNTER_HANDLER(x)       \
    if (name == #x) {            \
        CAT(data.m_, x) = value; \
    } else

        COUNTER_HANDLER(ctor)
        COUNTER_HANDLER(copy_ctor)
        COUNTER_HANDLER(move_ctor)
        COUNTER_HANDLER(dtor)
        COUNTER_HANDLER(copy_assign)
        COUNTER_HANDLER(move_assign)
        COUNTER_HANDLER(equals_compare)
        COUNTER_HANDLER(hash_calc) {
            throw std::string("what is counter::data_t name?");
        }

#undef COUNTER_HANDLER
    }
    return data;
}

inline std::ostream &operator<<(std::ostream &output, counter::data_t data) {
    return output << "{\"ctor\", " << data.m_ctor << "},\n"
                  << "{\"copy_ctor\", " << data.m_copy_ctor << "},\n"
                  << "{\"move_ctor\", " << data.m_move_ctor << "},\n"
                  << "{\"dtor\", " << data.m_dtor << "},\n"
                  << "{\"copy_assign\", " << data.m_copy_assign << "},\n"
                  << "{\"move_assign\", " << data.m_move_assign << "},\n"
                  << "{\"equals_compare\", " << data.m_equals_compare << "},\n"
                  << "{\"hash_calc\", " << data.m_hash_calc << "}\n";
}

namespace std {

template <>
struct hash<counter::object> {
    [[nodiscard]] std::size_t operator()(const counter::object &value
    ) const noexcept {
        return value.get_hash();
    }
};

}  // namespace std

#undef CAT
#undef CAT_IMPL