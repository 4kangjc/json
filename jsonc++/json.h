#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <map>

#include "value_t.h"
#include "iter_impl.h"
#include "marco.h"

namespace json {

class basic_value {
    template <typename BasicJsonType>
    friend class iter::impl;
public:
    using difference_type = std::ptrdiff_t;
    using value_type = basic_value;
    using reference  = value_type&;
    using const_reference = const value_type&;
    using iterator = iter::impl<basic_value>;
    using const_iterator = iter::impl<const basic_value>;
    using const_reverse_iterator = iter::reverse_iterator<const_iterator>;
    using reverse_iterator = iter::reverse_iterator<iterator>;

    template <class T>
    using AllocatorType = std::allocator<T>;
    using allocator_type = AllocatorType<basic_value>;
    using pointer = std::allocator_traits<allocator_type>::pointer;
    using const_pointer = std::allocator_traits<allocator_type>::const_pointer;

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

    template <class T> T as() const noexcept;
    template <class T> bool is() const noexcept;
    explicit operator bool() const { return !is_null(); }

    reference operator[](size_t idx);
    const_reference operator[](size_t idx) const;
    reference operator[](const object_t::key_type& key);
    const_reference operator[](const object_t::key_type& key) const;

    void push_back(basic_value&& val);
    void push_back(const basic_value& val);
    void push_back(const object_t::value_type& val);
    void push_back(object_t::value_type&& val);

    void reserve(size_t size);
    void resize(size_t size);
    void clear();

    template <class... Args>
    reference emplace_back(Args&&... args);

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args);

    template <class... Args>
    std::pair<basic_value::iterator, bool> try_emplace(Args&&... args);

    template <typename K>
    iterator find(K&& key);

    template <typename K>
    const_iterator find(K&& key) const;

    template <typename K>
    size_t count(K&& key) const;

    size_t size() const;
    bool empty() const;

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;
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


template <class... Args>
basic_value::reference basic_value::emplace_back(Args&&... args) {
    if (is_null()) {
        *this = basic_value(value_t::array);
    }
    if (is_array()) {
        return value_.array->emplace_back(std::forward<Args>(args)...);
    }
    JSON_ERROR_MSG(false, "cannot use emplace_back() for non-array value");
}

template <class... Args>
std::pair<basic_value::iterator, bool> basic_value::emplace(Args&&... args) {
    if (is_null()) {
        *this = basic_value(value_t::object);
    }
    if (is_object()) {
        auto [iter, ok] = value_.object->emplace(std::forward<Args>(args)...);
        iterator it(this);
        it.iter_ = iter;
        return {it, ok};
    }
    JSON_ERROR_MSG(false, "cannot use emplace() for non-object value");
}

template <class... Args>
std::pair<basic_value::iterator, bool> basic_value::try_emplace(Args&&... args) {
    if (is_null()) {
        *this = basic_value(value_t::object);
    }
    if (is_object()) {
        auto [iter, ok] = value_.object->try_emplace(std::forward<Args>(args)...);
        iterator it(this);
        it.iter_ = iter;
        return {it, ok};
    }
    JSON_ERROR_MSG(false, "cannot use try_emplace() for non-object value");
}

template <typename K>
basic_value::iterator basic_value::find(K&& key) {
    if (is_object()) {
        auto iter = value_.object->find(std::forward<K>(key));
        iterator it(this);
        it.iter_ = iter;
        return it;
    }
    JSON_ERROR_MSG(false, "cannot use find() for non-object value");
}

template <typename K>
basic_value::const_iterator basic_value::find(K&& key) const {
    if (is_object()) {
        auto iter = value_.object->find(std::forward<K>(key));
        const_iterator it(this);
        it.iter_ = iter;
        return it;
    }
    JSON_ERROR_MSG(false, "cannot use find() for non-object value");
}

template <typename K>
size_t basic_value::count(K&& key) const {
    if (is_object()) {
        return value_.object->count(std::forward<K>(key));
    }
    JSON_ERROR_MSG(false, "cannot use count() for non-object value");
}

template <> inline bool basic_value::as<bool>() const noexcept { return as_boolean(); }
template <> inline bool basic_value::is<bool>() const noexcept { return is_boolean(); }
template <> inline basic_value::num_int_t basic_value::as<basic_value::num_int_t>() const noexcept { return as_int(); }
template <> inline bool basic_value::is<basic_value::num_int_t>() const noexcept { return is_num_int(); }
template <> inline basic_value::num_uint_t basic_value::as<basic_value::num_uint_t>() const noexcept { return as_uint(); }
template <> inline bool basic_value::is<basic_value::num_uint_t>() const noexcept { return is_num_uint(); }
template <> inline float basic_value::as<float>() const noexcept { return as_float(); }
template <> inline double basic_value::as<double>() const noexcept { return as_double(); }
template <> inline bool basic_value::is<basic_value::num_real_t>() const noexcept { return is_real(); }
template <> inline const char* basic_value::as<const char*>() const noexcept { return as_cstring(); }
template <> inline basic_value::string_t basic_value::as<basic_value::string_t>() const noexcept { return as_string(); }
template <> inline bool basic_value::is<basic_value::string_t>() const noexcept { return is_string(); }
template <> inline bool basic_value::is<basic_value::array_t>() const noexcept { return is_array(); }
template <> inline bool basic_value::is<basic_value::object_t>() const noexcept { return is_object(); }


using value = basic_value;

}