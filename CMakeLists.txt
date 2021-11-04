cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# create project
project(micp)

# add executable
add_executable(micp main.cpp)

# add dependencies
include(cmake/CPM.cmake)
CPMAddPackage("gh:nlohmann/json@3.10.4")

# link dependencies
# target_link_libraries(tests Catch2)
