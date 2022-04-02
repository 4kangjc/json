cmake_minimum_required(VERSION 3.2)
project(jsoncc)

# include (cmake/utils.cmake)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}\
    -rdynamic\
    -O3\
    -ggdb\
    -std=c++17\
    -Wall\
    -Werror\
    -Wno-unused-function\
    -Wno-builtin-macro-redefined"
)

include_directories(./)


option(TESTS "flexy tests execute" OFF)

# redefine_file_macro(asco)
if(TESTS)
add_executable(test_json "tests/test_json.cc")

add_executable(test_parse "tests/test_parse.cc")

add_executable(test_meta "tests/test_meta.cc")

add_executable(test_read "tests/test_read.cc")

add_executable(test_speed "tests/test_speed.cc")
target_link_libraries(test_speed jsoncpp)

add_executable(test_write "tests/test_write.cc")
endif(TESTS)

add_executable(json "examples/json.cc")
add_executable(read "examples/read.cc")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
