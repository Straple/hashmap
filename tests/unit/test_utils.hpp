#pragma once

#include "hashmap.hpp"
//
#include "../counter/counter.hpp"
//
#include <catch.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

template <typename K = int, typename V = int, typename H = std::hash<K>>
using map_t = /*заменяем здесь*/ hashmap /*и тестируем*/<K, V, H>;
