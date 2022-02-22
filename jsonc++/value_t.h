#pragma once

#include <cstdint>

namespace json {

enum class value_t : std::uint8_t {
    null,
    object,
    array,
    string,
    boolean,
    number_int,
    number_uint,
    number_real,
};

}