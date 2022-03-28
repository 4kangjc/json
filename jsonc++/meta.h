#pragma once

#include <string>
#include <string_view>
#include <array>
#include <vector>

// can change charater          --> string
// forbid to change charater    --> cstring

namespace json::meta {

namespace detail {

template <typename T>
inline constexpr bool 
is_char_type_v = std::is_same_v<T, char>        || std::is_same_v<T, unsigned char> || 
                 std::is_same_v<T, signed char> || /*std::is_same_v<T, char8_t>     ||*/   // cpp20 
                 std::is_same_v<T, wchar_t>     || std::is_same_v<T, char16_t>      ||
                 std::is_same_v<T, char32_t>;

template <typename T, typename Enable = void>
struct is_string {
    static const bool value = false;
};

template <class _CharT, class _Traits, class _Alloc>
struct is_string<std::basic_string<_CharT, _Traits, _Alloc>> {
    static const bool value = true;
};

// template <>
template <typename _CharT>
struct is_string<std::vector<_CharT>, std::enable_if_t<is_char_type_v<std::remove_cv_t<_CharT>>>> {
    static const bool value = true;
};

template <typename T, typename Enable = void>
struct is_cstring {
    static const bool value = false;
};

// template <typename _CharT>
// struct is_cstring<const _CharT*, std::enable_if_t<is_char_type_v<std::remove_cv_t<_CharT>>>> {
//     static const bool value = true;
// };


// template <typename _CharT>
// struct is_cstring<_CharT*, std::enable_if_t<is_char_type_v<std::remove_cv<_CharT>>>> {
//     static const bool value = true;
// };

template <typename _CharPtr>
struct is_cstring<_CharPtr, std::enable_if_t<std::is_pointer_v<_CharPtr> 
                    && is_char_type_v<std::remove_cv_t<std::remove_pointer_t<_CharPtr>>>>> {
    static const bool value = true;
};

template <typename _CharT, std::size_t N>
struct is_cstring<std::array<_CharT, N>, std::enable_if_t<is_char_type_v<std::remove_cv_t<_CharT>>>> {
    static const bool value = true;
};


template <class _CharT, class _Traits>
struct is_cstring<std::basic_string_view<_CharT, _Traits>> {
    static const bool value = true;
};

} // namespace detail

template <class _Tp>
using is_string = detail::is_string<std::decay_t<_Tp>>;

template <class _Tp>
using is_cstring = detail::is_cstring<std::decay_t<_Tp>>;

template <class _Tp>
inline constexpr bool is_string_v = is_string<_Tp>::value;

template <class _Tp>
inline constexpr bool is_cstring_v = is_cstring<_Tp>::value;

template <typename T>
inline constexpr bool is_char_type_v = detail::is_char_type_v<std::decay_t<T>>;

}