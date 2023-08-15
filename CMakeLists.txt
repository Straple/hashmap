project(hashmap)

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD             17)
set(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)
set(GOOGLETEST_PATH "${CMAKE_SOURCE_DIR}/contrib/googletest" CACHE PATH "")

find_package(Catch REQUIRED)

add_subdirectory("${CMAKE_SOURCE_DIR}/contrib/benchmark")

include(cmake/TestSolution.cmake)
include(cmake/BuildFlags.cmake)

find_package(PNG)

if (NOT DEFINED PNG_FOUND)
    message(FATAL_ERROR "Can't find libpng")
endif()

add_catch(test_hashmap tests/benchmark/test.cpp)

add_catch(test_unit
        tests/unit/ctors.cpp

        tests/unit/insert.cpp
        tests/unit/emplace.cpp
        tests/unit/erase.cpp

        tests/unit/assign_to_move.cpp
        tests/unit/assignment_combinations.cpp

        tests/unit/load_factor.cpp

        tests/unit/count.cpp
        tests/unit/contains.cpp
        tests/unit/at.cpp

        tests/unit/diamond.cpp

        tests/unit/unique_ptr.cpp

        tests/unit/empty.cpp

        tests/unit/noncopyable.cpp
        tests/unit/std_string.cpp
        )

add_catch(counter_unit_test
        tests/counter/test.cpp
        )

add_executable(main main.cpp)