cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# create project
project(micp)

# add executable
add_executable(micp main.cpp)

# add dependencies
include(cmake/CPM.cmake)
CPMAddPackage("gh:nlohmann/json@3.10.4")

# use new cool stuff
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(micp PRIVATE nlohmann_json::nlohmann_json)
