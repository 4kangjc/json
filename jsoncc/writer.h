#pragma once

#include "output_adapters.h"
#include "noncopyable.h"
#include <charconv>

namespace json {


#define write_literal_value(value, length)                                                          \
    if constexpr (std::is_same_v<CharType, wchar_t>) {                                              \
        oa->write_characters(L##value, length);                                                     \
    } else {                                                                                        \
        if constexpr (sizeof(CharType) == 1) {                                                      \
            if constexpr (std::is_same_v<CharType, char>) {                                         \
                oa->write_characters(value, length);                                                \
            } else {                                                                                \
                oa->write_characters(reinterpret_cast<const CharType*>(value), length);             \
            }                                                                                       \
        } else if constexpr (sizeof(CharType) == 2) {                                               \
            oa->write_characters(u##value, length);                                                 \
        } else if constexpr (sizeof(CharType) == 4) {                                               \
            oa->write_characters(U##value, length);                                                 \
        }                                                                                           \
    }                                                                                              



template <typename BasicJsonType, bool pretty = false, typename CharType = char>
class writer : noncopyable {
public:
    using string_t = typename BasicJsonType::string_t;
    using num_real_t = typename BasicJsonType::num_real_t;
    using num_int_t = typename BasicJsonType::num_int_t;
    using num_uint_t = typename BasicJsonType::num_uint_t;
public:
    writer(output_adapter<CharType>&& s)    
        : oa(std::move(s)) 
    {}

    writer(const output_adapter<CharType>& s)
        : oa(s)
    {}

    ~writer() = default;

    void dump(const BasicJsonType& json) {
        if constexpr (pretty) {
            __write(json, 0);
        } else {
            __write(json);
        }
    }
private:

                                                                         

    void __write(const BasicJsonType& json) {
        switch (json.type_) {
            case value_t::object: {
                oa->write_character('{');
                bool first = true;
                for (const auto& [key, value] : *json.value_.object) {
                    if (first) {
                        first = false;
                    } else {
                        oa->write_character(',');
                    }
                    oa->write_character('\"');
                    oa->write_characters(key.data(), key.size());
                    // oa->write_characters("\":", 2);
                    oa->write_character('\"');
                    oa->write_character(':');
                    __write(value);
                }
                oa->write_character('}');
                break;
            }
            case value_t::array: {
                oa->write_character('[');
                bool first = true;
                for (const auto& value : *json.value_.array) {
                    if (first) {
                        first = false;
                    } else {
                        oa->write_character(',');
                    }
                    __write(value);
                }
                oa->write_character(']');
                break;
            }
            case value_t::null: {
                // oa->write_characters("null", 4);
                write_literal_value("null", 4)
                break;
            }
            case value_t::boolean: {
                if (json.value_.boolean) {
                    // oa->write_characters("true", 4);
                    write_literal_value("true", 4)
                } else {
                    // oa->write_characters("false", 5);
                    write_literal_value("false", 5)
                }
                break;
            }
            case value_t::number_int: {
                if constexpr (sizeof(CharType) == 1) {
                    string_t num_string;
                    if constexpr (sizeof(num_int_t) <= 4) {
                        num_string.resize(32);
                    } else if constexpr (sizeof(num_int_t) <= 8) {
                        num_string.resize(64);
                    } else {
                        num_string.resize(128);
                    }
                    auto begin = (char*)num_string.data();
                    auto end   = (char*)num_string.data() + num_string.size();
                    auto [ptr, ec] = std::to_chars(begin, end, json.value_.num_int);
                    if (JSON_LIKELY(ec == std::errc())) {
                        oa->write_characters(num_string.data(), ptr - begin);
                    } else {

                    }
                } else {
                    std::basic_stringstream<CharType> ss;
                    ss << json.value_.num_int;
                    // ss >> num_string;
                    string_t num_string = ss.str();
                    oa->write_characters(num_string.data(), num_string.size());
                }
                break;
            }
            case value_t::number_uint: {
                if constexpr (sizeof(CharType) == 1) {
                    string_t num_string;
                    if constexpr (sizeof(num_uint_t) <= 4) {
                        num_string.resize(32);
                    } else if constexpr (sizeof(num_uint_t) <= 8) {
                        num_string.resize(64);
                    } else {
                        num_string.resize(128);
                    }
                    auto begin = (char*)num_string.data();
                    auto end   = (char*)num_string.data() + num_string.size();
                    auto [ptr, ec] = std::to_chars(begin, end, json.value_.num_uint);
                    if (JSON_LIKELY(ec == std::errc())) {
                        oa->write_characters(num_string.data(), ptr - begin);
                    } else {

                    }
                } else {
                    std::basic_stringstream<CharType> ss;
                    ss << json.value_.num_uint;
                    string_t num_string = ss.str();
                    // ss >> num_string;
                    // oa->write_characters(ss.str(), ss.str().size());
                    oa->write_characters(num_string.data(), num_string.size());
                }
                break;
            }
            case value_t::number_real: {
                if constexpr (sizeof(CharType) == 1) {
                    string_t num_string;
                    if constexpr (sizeof(num_real_t) <= 4) {
                        num_string.resize(32);
                    } else if constexpr (sizeof(num_real_t) <= 8) {
                        num_string.resize(64);
                    } else {
                        num_string.resize(128);
                    }
                    auto begin = (char*)num_string.data();
                    auto end   = (char*)num_string.data() + num_string.size();
                    auto [ptr, ec] = std::to_chars(begin, end, json.value_.num_real);
                    if (JSON_LIKELY(ec == std::errc())) {
                        oa->write_characters(num_string.data(), ptr - begin);
                    } else {

                    }
                } else {
                    std::basic_stringstream<CharType> ss;
                    ss << json.value_.num_real;
                    string_t num_string = ss.str();
                    ss >> num_string;
                    // oa->write_characters(ss.str(), ss.str().size());
                    oa->write_characters(num_string.data(), num_string.size());
                }
                break;
            }
            case value_t::string: {
                oa->write_character('\"');
                oa->write_characters(json.value_.string->data(), json.value_.string->size());
                oa->write_character('\"');
            }
            default:
                break;
        }
    }

    void __write(const BasicJsonType& json, unsigned int level) {

    }

#undef write_literal_value

private:
    output_adapter<CharType> oa = nullptr;
    std::conditional_t<pretty, string_t, char> indent_string;
};

} // namespace json