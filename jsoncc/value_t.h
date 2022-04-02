#pragma once

#include <cstdint>
#include <array>

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

inline bool operator<(const value_t lhs, const value_t rhs) noexcept {
    static constexpr std::array<std::uint8_t, 9> order = {
        0 /* null */, 3 /* object */, 4 /* array */, 5 /* string */,
        1 /* boolean */, 2 /* integer */, 2 /* unsigned */, 2 /* float */,
    };

    const auto l_index = static_cast<std::size_t>(lhs);
    const auto r_index = static_cast<std::size_t>(rhs);
    return l_index < order.size() && r_index < order.size() && order[l_index] < order[r_index];
}

} // namespace json