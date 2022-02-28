#pragma once

#include <string>
#include <string_view>

namespace json::meta {

template <typename T>
struct is_string {
    static const bool value = false;
};

template <class T, class Traits, class Alloc>
struct is_string<std::basic_string<T, Traits, Alloc>> {
    static const bool value = true;
};

template <typename T>
struct is_cstring {
    static const bool value = false;
};

template <>
struct is_cstring<const char*> {
    static const bool value = true;
};

template <>
struct is_cstring<const char[]> {
    static const bool value = true;
};

template <std::size_t N>
struct is_cstring<const char(&)[N]> {
    static const bool value = true;
};

template <std::size_t N>
struct is_cstring<const char[N]> {
    static const bool value = true;
};

template <>
struct is_cstring<char*> {
    static const bool value = true;
};

template <>
struct is_cstring<char[]> {
    static const bool value = true;
};

template <std::size_t N>
struct is_cstring<char[N]> {
    static const bool value = true;
};

template <std::size_t N>
struct is_cstring<char(&)[N]> {
    static const bool value = true;
};

template <class T, class Traits>
struct is_cstring<std::basic_string_view<T, Traits>> {
    static const bool value = true;
};

template <class _Tp>
inline constexpr bool is_string_v = is_string<_Tp>::value;

template <class _Tp>
inline constexpr bool is_cstring_v = is_cstring<_Tp>::value;

}