#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <map>

#include "iter_impl.h"

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

class basic_value {
public:
    using value_type = basic_value;
    using reference  = value_type&;
    using const_reference = const value_type&;
    using iterator = iter::impl<basic_value>;
    using const_iterator = iter::impl<const basic_value>;
    using const_reverse_iterator = iter::reverse_iterator<const_iterator>;
    using reverse_iterator = iter::reverse_iterator<iterator>;

    template <class T>
    using AllocatorType = std::allocator<T>;

    using object_t    = std::map<std::string, basic_value, std::less<>, 
                        AllocatorType<std::pair<const std::string, basic_value>>>;
    using array_t     = std::vector<basic_value, AllocatorType<basic_value>>;
    using string_t    = std::string;
    using boolean_t   = bool;
    using num_int_t   = int;
    using num_uint_t  = uint;
    using num_real_t  = double;

    basic_value() = default;
    basic_value(value_t type);
    basic_value(boolean_t v);
    basic_value(num_int_t v);
    basic_value(num_uint_t v);
    basic_value(const char* v) : basic_value(string_t(v)) {}
    basic_value(const string_t& v);
    basic_value(string_t&& v);
    basic_value(num_real_t v);

    basic_value(basic_value&& rhs);
    basic_value& operator=(basic_value&& rhs);

    ~basic_value() { destory(); }

    const char* type_name() const noexcept;
    constexpr value_t type() const noexcept { return type_; }
    constexpr bool is_null() const noexcept { return type_ == value_t::null; }
    constexpr bool is_number() const noexcept { return is_num_int() || is_num_uint() || is_real(); }
    constexpr bool is_num_int() const noexcept { return type_ == value_t::number_int; }
    constexpr bool is_num_uint() const noexcept { return type_ == value_t::number_uint; }
    constexpr bool is_real() const noexcept { return type_ == value_t::number_real; }
    constexpr bool is_array() const noexcept { return type_ == value_t::array; }
    constexpr bool is_object() const noexcept { return type_ == value_t::object; }
    constexpr bool is_boolean() const noexcept { return type_ == value_t::boolean; }
    constexpr bool is_string() const noexcept { return type_ == value_t::string; }

    bool as_boolean() const;
    const char* as_cstring() const;
    string_t as_string() const;
    float as_float() const;
    double as_double() const;
    num_int_t as_int() const;
    num_uint_t as_uint() const;

    reference operator[](size_t idx);
    const_reference operator[](size_t idx) const;
    reference operator[](const object_t::key_type& key);
    const_reference operator[](const object_t::key_type& key) const;

    void push_back(basic_value&& val);
    void push_back(const basic_value& val);
    void push_back(const object_t::value_type& val);
    void push_back(object_t::value_type&& val);



    template <class... Args>
    reference emplace_back(Args&&... args);

    template <class... Args>
    std::pair<object_t::iterator, bool> emplace(Args&&... args);

private:
    void destory();

private:
    union json_value {
        object_t* object;
        array_t*  array;
        string_t* string = nullptr;
        boolean_t boolean;
        num_int_t num_int;
        num_uint_t num_uint;
        num_real_t num_real;

        json_value(value_t type);
        json_value(nullptr_t = nullptr) : object(nullptr) {}
        json_value(boolean_t v) noexcept : boolean(v) {}
        json_value(num_int_t v) noexcept : num_int(v) {}
        json_value(num_uint_t v) noexcept : num_uint(v) {}
        json_value(num_real_t v) noexcept : num_real(v) {}
        json_value(const string_t& v) { 
            if (string) {
                delete string;
                *string = v; 
            } else {
                string = new string_t(v);
            }
        }
        json_value(string_t&& v) noexcept { 
            if (string) {
                delete string; 
                *string = std::move(v);
            } else {
                string = new string_t(std::move(v));
            }
        }

    };
    value_t type_ = value_t::null;
    json_value value_{};

};

using value = basic_value;

}