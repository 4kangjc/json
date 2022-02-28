cmake_minimum_required(VERSION 3.2)
project(jsonc++)

# include (cmake/utils.cmake)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}\
    -rdynamic\
    -O0\
    -ggdb\
    -std=c++17\
    -Wall\
    -Werror\
    -Wno-unused-function\
    -Wno-builtin-macro-redefined"
)

include_directories(./)

set(LIB_SRC
    jsonc++/util.cc
)



set(LIBS
    jsonc++
)

option(BUILD_SHARED_LIBS "Build json as a shared lib" ON)
option(BUILD_STATIC_LIBS "Build json as a static lib" OFF)


if (BUILD_STATIC_LIBS)
add_library(jsonc++ STATIC ${LIB_SRC})
elseif (BUILD_SHARED_LIBS)
add_library(jsonc++ SHARED ${LIB_SRC})
endif(BUILD_STATIC_LIBS)

# redefine_file_macro(asco)

add_executable(test_json "tests/test_json.cc")
target_link_libraries(test_json jsonc++)

add_executable(test_parse "tests/test_parse.cc")
target_link_libraries(test_parse jsonc++)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
