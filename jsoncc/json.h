#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <map>
#include <optional>

#include "input_adapters.h"
#include "value_t.h"
#include "iter_impl.h"
#include "marco.h"
#include "meta.h"
#include "reader.h"
#include "writer.h"

namespace json {

#define BASIC_VALUE_TPL_DECL                                                \
template <template<class, class, typename...> class ObjectType,             \
          template<class, class...> class ArrayType,                        \
          class StringType,                                                 \
          class BooleanType,                                                \
          class NumberIntegerType,                                          \
          class NumberUnsignedType,                                         \
          class NumberRealType,                                             \
          template<class> class AllocatorType                               \
          >

#define BASIC_VALUE_TPL basic_value<ObjectType, ArrayType, StringType, BooleanType, NumberIntegerType, NumberUnsignedType, NumberRealType, AllocatorType> 

template <template<class, class, typename...> class ObjectType = std::map,
          template<class, class...> class ArrayType = std::vector,
          class StringType = std::string,
          class BooleanType = bool,
          class NumberIntegerType = std::int64_t,
          class NumberUnsignedType = std::uint64_t,
          class NumberRealType = double,
          template<class> class AllocatorType = std::allocator
          >
// BASIC_VALUE_TPL_DECL
class basic_value {
private:
    template <typename BasicJsonType>
    friend class iter::impl;

    template <typename BasicJsonType, typename InputType, bool ignore_comments>
    friend class reader;

    template<typename BasicJsonType>
    friend class json_sax_dom_parser;

    template <typename BasicJsonType, bool , typename CharType>
    friend class writer;
public:
    using difference_type = std::ptrdiff_t;
    using value_type = basic_value;
    using reference  = value_type&;
    using const_reference = const value_type&;
    using iterator = iter::impl<basic_value>;
    using const_iterator = iter::impl<const basic_value>;
    using const_reverse_iterator = iter::reverse_iterator<const_iterator>;
    using reverse_iterator = iter::reverse_iterator<iterator>;

    using allocator_type = AllocatorType<basic_value>;
    using pointer =  typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    using object_t    = ObjectType<StringType, basic_value, std::less<>,
                        AllocatorType<std::pair<const StringType, basic_value>>>;
    using array_t     = ArrayType<basic_value, AllocatorType<basic_value>>;
    using string_t    = StringType;
    using boolean_t   = BooleanType;
    using num_int_t   = NumberIntegerType;
    using num_uint_t  = NumberUnsignedType;
    using num_real_t  = NumberRealType;

    basic_value(std::nullptr_t = nullptr) {}
    basic_value(value_t type);

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> 
                                                    || meta::is_string_v<T> || meta::is_cstring_v<T>>>
    basic_value(T&& v) {
        if constexpr (std::is_integral_v<T>) {
            if constexpr (std::is_same_v<T, boolean_t>) {
                value_.boolean = v;
                type_ = value_t::boolean;
            } else if constexpr (std::is_signed_v<T>) {
                value_.num_int = v;
                type_ = value_t::number_int;
            } else {
                value_.num_uint = v;
                type_ = value_t::number_uint;
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            value_.num_real = v;
            type_ = value_t::number_real;
        } else if constexpr (meta::is_string_v<T>) {
            value_.string = new string_t(std::forward<T>(v));
            type_ = value_t::string;
        } else if constexpr (meta::is_cstring_v<T>) {
            value_.string = new string_t(v);
            type_ = value_t::string;
        } else {
            // TODO LOG
            JSON_LOG("invalid type create basic_value\n");
        }
    }

    basic_value(basic_value&& rhs) noexcept ;
    basic_value& operator=(basic_value&& rhs) noexcept ;
    basic_value& operator=(nullptr_t null) { destory(); return *this; }
    basic_value(const basic_value& rhs);
    basic_value& operator=(const basic_value& rhs);

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
    //const char* as_cstring() const;
    const typename string_t::value_type* as_cstring() const;
    // char* as_cstring();     // is not const !!! can write
    string_t as_string() const;
    float as_float() const;
    double as_double() const;
    num_int_t as_int() const;
    num_uint_t as_uint() const;
    num_real_t as_real() const;

    template <class T> T as();
    template <class T> bool is() const noexcept;
    explicit operator bool() const { return !is_null(); }

    void reset() { destory(); }

    reference operator[](size_t idx);
    const_reference operator[](size_t idx) const;
    reference operator[](const typename object_t::key_type& key);
    reference operator[](typename object_t::key_type&& key);
    const_reference operator[](const typename object_t::key_type& key) const;

    void push_back(basic_value&& val);
    void push_back(const basic_value& val);
    void push_back(const typename object_t::value_type& val);
    void push_back(typename object_t::value_type&& val);

    void pop_back();

    void reserve(size_t size);
    void resize(size_t size);
    void clear() noexcept;

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

    iterator erase(iterator pos);
    const_iterator erase(const_iterator pos);
    iterator erase(iterator begin, iterator end);
    const_iterator erase(const_iterator begin, const_iterator end);

    template <typename K>
    size_t erase(K&& key);

    template <typename K>
    size_t count(K&& key) const;

    size_t size() const;
    bool empty() const;

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;
    
    reference front() { return *begin(); }
    const_reference front() const { return *cbegin(); }
    reference back() { return *--end(); }
    const_reference back() const { return *--cend(); }

public:
    template <bool ignore_comment = false, typename InputType>
    static std::optional<basic_value> parse(InputType&& i, parser_callback_t<basic_value>&& cb = nullptr) {
        basic_value result;
#define adapter input_adapter(std::forward<InputType>(i))
        using InputAdapterType = decltype(adapter);
        auto reader = json::reader<basic_value, InputAdapterType, ignore_comment>(adapter, std::move(cb));
        auto ok = reader.parse(result);
#undef adapter
        if (ok) {
            return result;
        }
        JSON_LOG("[ERROR]: \n" + reader.get_error_msg());
        return {};
    }


    template <bool ignore_commet = false, typename IteratorType>
    static std::optional<basic_value> parse(IteratorType begin, IteratorType end, parser_callback_t<basic_value>&& cb = nullptr) {
        basic_value result;
#define adapter input_adapter(std::move(begin), std::move(end)) 
        using InputAdapterType = decltype(adapter);
        auto reader = json::reader<basic_value, InputAdapterType, ignore_commet>(adapter, std::move(cb));
        auto ok = parse(result);
#undef adapter
        if (ok) {
            return result;
        }
        JSON_LOG("[ERROR]: \n" + reader.get_error_msg());
        return {};
    }

    std::ostream& dump(std::ostream& os) const {
        switch (type_) {
            case value_t::array: {
                os << '[';
                bool first = true;
                for (const auto& value : *value_.array) {
                    if (first) {
                        first = false;
                    } else {
                        os << ',';
                    }
                    value.dump(os);
                }
                os << ']';
                break;
            }
            case value_t::object: {
                os << '{';
                bool first = true;
                for (const auto& [key, value] : *value_.object) {
                    if (first) {
                        first = false;
                    } else {
                        os << ',';
                    }
                    os << '\"' << key << "\":";
                    value.dump(os);
                }
                os << '}';
                break;
            }
            case value_t::null: {
                os << "null";
                break;
            }
            case value_t::boolean: {
                os << (value_.boolean ? "true" : "false");
                break;
            }
            case value_t::number_int: {
                os << as_int();
                break;
            }
            case value_t::number_uint: {
                os << as_uint();
                break;
            }
            case value_t::number_real: {
                os << as_real();
                break;
            }
            case value_t::string: {
                os << '\"' << as_string() << '\"';
                break;
            }
            default:
                break;
        }
        return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const basic_value& json)  {
        return json.dump(os);
    }

    std::wostream& dump(std::wostream& os) const {
        switch (type_) {
            case value_t::array: {
                os << L'[';
                bool first = true;
                for (const auto& value : *value_.array) {
                    if (first) {
                        first = false;
                    } else {
                        os << L',';
                    }
                    value.dump(os);
                }
                os << L']';
                break;
            }
            case value_t::object: {
                os << L'{';
                bool first = true;
                for (const auto& [key, value] : *value_.object) {
                    if (first) {
                        first = false;
                    } else {
                        os << L',';
                    }
                    os << L'\"' << key << L"\":";
                    value.dump(os);
                }
                os << L'}';
                break;
            }
            case value_t::null: {
                os << L"null";
                break;
            }
            case value_t::boolean: {
                os << (value_.boolean ? L"true" : L"false");
                break;
            }
            case value_t::number_int: {
                os << as_int();
                break;
            }
            case value_t::number_uint: {
                os << as_uint();
                break;
            }
            case value_t::number_real: {
                os << as_real();
                break;
            }
            case value_t::string: {
                os << L'\"' << as_string() << L'\"';
                break;
            }
            default:
                break;
        }
        return os;
    }
     
    template <bool pretty = false, typename CharType = typename string_t::value_type,
              typename Output, typename = std::enable_if_t<
              std::is_convertible_v<Output, output_adapter<CharType>>>>
    void write(Output&& op) const {
        json::writer<basic_value, pretty, CharType> writer(std::forward<Output>(op));
        writer.dump(*this);
    }
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
        // json_value(boolean_t v) noexcept : boolean(v) {}
        // json_value(num_int_t v) noexcept : num_int(v) {}
        // json_value(num_uint_t v) noexcept : num_uint(v) {}
        // json_value(num_real_t v) noexcept : num_real(v) {}
        // json_value(const char* v) noexcept : json_value(string_t(v)) { }
        // json_value(const string_t& v) { string = new string_t(v); }
        // json_value(string_t&& v) noexcept { string = new string_t(std::move(v)); }
    };

    value_t type_ = value_t::null;
    json_value value_{};

};



BASIC_VALUE_TPL_DECL
template <class... Args>
typename BASIC_VALUE_TPL::reference BASIC_VALUE_TPL::emplace_back(Args&&... args) {
    if (is_null()) {
        *this = basic_value(value_t::array);
    }
    if (JSON_LIKELY(is_array())) {
        return value_.array->emplace_back(std::forward<Args>(args)...);
    }
    JSON_ERROR_MSG(false, "cannot use emplace_back() for non-array value");
}

BASIC_VALUE_TPL_DECL
template <class... Args>
std::pair<typename BASIC_VALUE_TPL::iterator, bool> BASIC_VALUE_TPL::emplace(Args&&... args) {
    if (is_null()) {
        *this = basic_value(value_t::object);
    }
    if (JSON_LIKELY(is_object())) {
        auto [iter, ok] = value_.object->emplace(std::forward<Args>(args)...);
        iterator it(this);
        it.iter_ = iter;
        return {it, ok};
    }
    JSON_ERROR_MSG(false, "cannot use emplace() for non-object value");
}

BASIC_VALUE_TPL_DECL
template <class... Args>
std::pair<typename BASIC_VALUE_TPL::iterator, bool> BASIC_VALUE_TPL::try_emplace(Args&&... args) {
    if (is_null()) {
        *this = basic_value(value_t::object);
    }
    if (JSON_LIKELY(is_object())) {
        auto [iter, ok] = value_.object->try_emplace(std::forward<Args>(args)...);
        iterator it(this);
        it.iter_ = iter;
        return {it, ok};
    }
    JSON_ERROR_MSG(false, "cannot use try_emplace() for non-object value");
}

#define __erase__(pos)                                                      \
    switch (type_) {                                                        \
        case value_t::object:                                               \
            result.iter_ = value_.object->erase(std::get<0>(pos.iter_));    \
            return result;                                                  \
        case value_t::array:                                                \
            result.iter_ = value_.array->erase(std::get<1>(pos.iter_));     \
            return result;                                                  \
        default:                                                            \
            break;                                                          \
    }                                                                       \
    JSON_ERROR_MSG(false, "invalid type use erase, type = " << type_name())


BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::iterator BASIC_VALUE_TPL::erase(typename BASIC_VALUE_TPL::iterator pos) {
    iterator result(this);
    __erase__(pos);
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_iterator BASIC_VALUE_TPL::erase(typename BASIC_VALUE_TPL::const_iterator pos) {
    const_iterator result(this);
    __erase__(pos);
}

#undef __erase__

#define __erase__(begin, end)                                                                       \
    switch (type_) {                                                                                \
        case value_t::object:                                                                       \
            result.iter_ = value_.object->erase(std::get<0>(begin.iter_), std::get<0>(end.iter_));  \
            return result;                                                                          \
        case value_t::array:                                                                        \
            result.iter_ = value_.array->erase(std::get<1>(begin.iter_), std::get<1>(end.iter_));   \
            return result;                                                                          \
        default:                                                                                    \
            break;                                                                                  \
    }                                                                                               \
    JSON_ERROR_MSG(false, "invalid type use erase, type = " << type_name())

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::iterator BASIC_VALUE_TPL::erase(typename BASIC_VALUE_TPL::iterator begin, typename BASIC_VALUE_TPL::iterator end) {
    iterator result(this);
    __erase__(begin, end);
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_iterator BASIC_VALUE_TPL::erase(typename BASIC_VALUE_TPL::const_iterator begin, typename BASIC_VALUE_TPL::const_iterator end) {
    const_iterator result(this);
    __erase__(begin, end);
}

#undef __erase__

BASIC_VALUE_TPL_DECL
template <typename K>
size_t BASIC_VALUE_TPL::erase(K&& key) {
    if (JSON_LIKELY(is_object())) {
        return value_.object->erase(std::forward<K>(key));
    }
    JSON_ERROR_MSG(false, "cannot use find() for non-object value");
}

BASIC_VALUE_TPL_DECL
template <typename K>
typename BASIC_VALUE_TPL::iterator BASIC_VALUE_TPL::find(K&& key) {
    if (JSON_LIKELY(is_object())) {
        auto iter = value_.object->find(std::forward<K>(key));
        iterator it(this);
        it.iter_ = iter;
        return it;
    }
    JSON_ERROR_MSG(false, "cannot use find() for non-object value");
}

BASIC_VALUE_TPL_DECL
template <typename K>
typename BASIC_VALUE_TPL::const_iterator BASIC_VALUE_TPL::find(K&& key) const {
    if (JSON_LIKELY(is_object())) {
        auto iter = value_.object->find(std::forward<K>(key));
        const_iterator it(this);
        it.iter_ = iter;
        return it;
    }
    JSON_ERROR_MSG(false, "cannot use find() for non-object value");
}

BASIC_VALUE_TPL_DECL
template <typename K>
size_t BASIC_VALUE_TPL::count(K&& key) const {
    if (JSON_LIKELY(is_object())) {
        return value_.object->count(std::forward<K>(key));
    }
    JSON_ERROR_MSG(false, "cannot use count() for non-object value");
}

BASIC_VALUE_TPL_DECL
BASIC_VALUE_TPL::json_value::json_value(value_t type) {
    switch (type) {
        case value_t::null:
            object = nullptr;
            break;
        case value_t::boolean:
            boolean = false;
            break;
        case value_t::number_int:
            num_int = 0;
            break;
        case value_t::number_uint:
            num_uint = 0;
            break;
        case value_t::number_real:
            num_real = 0;
            break;
        case value_t::string:
            string = new string_t();
            break;
        case value_t::array:
            array = new array_t();
            break;
        case value_t::object:
            object = new object_t();
            break;
        default:
            JSON_ERROR_MSG(false, "invalid type to json_value");
    }
}

BASIC_VALUE_TPL_DECL
BASIC_VALUE_TPL::basic_value(value_t type)
    : type_(type), value_(type) {

}

BASIC_VALUE_TPL_DECL
BASIC_VALUE_TPL::basic_value(basic_value&& rhs) noexcept {
    value_ = rhs.value_;
    type_ = rhs.type_;
    rhs.value_ = {};
    rhs.type_ = value_t::null;
}

BASIC_VALUE_TPL_DECL
BASIC_VALUE_TPL& BASIC_VALUE_TPL::operator=(basic_value&& rhs) noexcept {
    if (this != &rhs) {
        destory();
        value_ = rhs.value_;
        type_ = rhs.type_;
        rhs.value_ = {};
        rhs.type_ = value_t::null;
    }
    return *this;
}

BASIC_VALUE_TPL_DECL
const char* BASIC_VALUE_TPL::type_name() const noexcept {
    switch (type_) {
        case value_t::null:
            return "null";
        case value_t::object:
            return "object";
        case value_t::array:
            return "array";
        case value_t::boolean:
            return "boolean";
        case value_t::number_int:
        case value_t::number_uint:
        case value_t::number_real:
            return "number";
        case value_t::string:
            return "string";
        default:
            return "other";
    }
    return "";
}

BASIC_VALUE_TPL_DECL
bool BASIC_VALUE_TPL::as_boolean() const {
    switch (type_) {
        case value_t::null:
            return false;
        case value_t::boolean:
            return value_.boolean;
        case value_t::number_int:
            return value_.num_int;
        case value_t::number_uint:
            return value_.num_uint;
        case value_t::number_real:
            return value_.num_real;
        default:
            break;
    }
    JSON_ERROR_MSG(false, "value can't convertible to bool, type = " << type_name());
    return false;
}

BASIC_VALUE_TPL_DECL
const typename BASIC_VALUE_TPL::string_t::value_type* BASIC_VALUE_TPL::as_cstring() const {
    if (JSON_LIKELY(is_string())) {
        // return value_.string->c_str();
        return value_.string->data();
    }
    JSON_ERROR_MSG(false, "value_type is not string, type = " << type_name());
    return nullptr;
}

// BASIC_VALUE_TPL_DECL
// char* BASIC_VALUE_TPL::as_cstring() {
//     if (JSON_LIKELY(is_string())) {
//         return value_.string->data();
//     }
//     JSON_ERROR_MSG(false, "value_type is not string, type = " << type_name());
//     return nullptr;
// }

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::string_t BASIC_VALUE_TPL::as_string() const {
    if constexpr (std::is_same_v<string_t, std::string>) {
        switch (type_) {
            case value_t::null:
                return "null";
            case value_t::boolean:
                return value_.boolean ? "true" : "false";
            case value_t::number_int:
                return std::to_string(value_.num_int);
                return string_t();
            case value_t::number_uint:
                return std::to_string(value_.num_uint);
            case value_t::number_real:
                return std::to_string(value_.num_real);
            case value_t::string:
                return *value_.string;
            default:
                break;
        }
    } else {
        if (JSON_LIKELY(type_ == value_t::string)) {
            return *value_.string;
        }
    }
    JSON_ERROR_MSG(false, "value can't convertible to string, type = " << type_name());
    return string_t();
}

BASIC_VALUE_TPL_DECL
float BASIC_VALUE_TPL::as_float() const {
    switch (type_) {
        case value_t::null:
            return 0.0;
        case value_t::boolean:
            return value_.boolean;
        case value_t::number_int:
            return value_.num_int;
        case value_t::number_uint:
            return value_.num_uint;
        case value_t::number_real:
            return value_.num_real;
        default:
            break;
    }
    JSON_ERROR_MSG(false, "value can't convertible to float, type = " << type_name());
    return 0;
}

BASIC_VALUE_TPL_DECL
double BASIC_VALUE_TPL::as_double() const {
    switch (type_) {
        case value_t::null:
            return 0.0;
        case value_t::boolean:
            return value_.boolean;
        case value_t::number_int:
            return value_.num_int;
        case value_t::number_uint:
            return value_.num_uint;
        case value_t::number_real:
            return value_.num_real;
        default:
            break;
    }
    JSON_ERROR_MSG(false, "value can't convertible to double, type = " << type_name());
    return 0;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::num_real_t BASIC_VALUE_TPL::as_real() const {
    switch (type_) {
        case value_t::null:
            return 0.0;
        case value_t::boolean:
            return value_.boolean;
        case value_t::number_int:
            return value_.num_int;
        case value_t::number_uint:
            return value_.num_uint;
        case value_t::number_real:
            return value_.num_real;
        default:
            break;
    }
    JSON_ERROR_MSG(false, "value can't convertible to double, type = " << type_name());
    return 0;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::num_int_t BASIC_VALUE_TPL::as_int() const {
    switch (type_) {
        case value_t::number_int:
            return value_.num_int;
        case value_t::number_uint:
            return value_.num_uint;
        case value_t::number_real:
            return value_.num_real;
        case value_t::boolean:
            return value_.boolean;
        case value_t::null:
            return 0;
        default:
            break;
    }
    JSON_ERROR_MSG(false, "value can't convertible to int, type = " << type_name());
    return 0;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::num_uint_t BASIC_VALUE_TPL::as_uint() const {
    switch (type_) {
        case value_t::number_int:
            return value_.num_int;
        case value_t::number_uint:
            return value_.num_uint;
        case value_t::number_real:
            return value_.num_real;
        case value_t::boolean:
            return value_.boolean;
        case value_t::null:
            return 0;
        default:
            break;
    }
    JSON_ERROR_MSG(false, "value can't convertible to uint, type = " << type_name());
    return 0;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::reference BASIC_VALUE_TPL::operator[](size_t idx) {
    switch (type_) {
        case value_t::null:
            *this = basic_value(value_t::array);
        case value_t::array:
            if (value_.array->size() <= idx)
                value_.array->resize(idx + 1);
            return value_.array->operator[](idx);
        default:
            JSON_ERROR_MSG(false, "invalid type use operator[], type = " << type_name());
    }
}


BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_reference BASIC_VALUE_TPL::operator[](size_t idx) const {
    if (JSON_LIKELY(is_array())) {
        if (value_.array->size() <= idx) {
            JSON_ERROR_MSG(false, "out of range, array size = " << value_.array->size()
            << ", index = " << idx);
        }
        return value_.array->operator[](idx);
    }
    JSON_ERROR_MSG(false, "invalid type use operator[], type = " << type_name());
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::reference BASIC_VALUE_TPL::operator[](const typename object_t::key_type& key) {
    switch (type_) {
        case value_t::null:
            *this = basic_value(value_t::object);
        case value_t::object:
            return value_.object->operator[](key);
        default:
            JSON_ERROR_MSG(false, "invalid type use operator[], type = " << type_name());
    }
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::reference BASIC_VALUE_TPL::operator[](typename object_t::key_type&& key) {
    switch (type_) {
        case value_t::null:
            *this = basic_value(value_t::object);
        case value_t::object:
            return value_.object->operator[](std::move(key));
        default:
            JSON_ERROR_MSG(false, "invalid type use operator[], type = " << type_name());
    }
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_reference BASIC_VALUE_TPL::operator[](const typename object_t::key_type& key) const {
    if (JSON_LIKELY(is_object())) {
        return value_.object->operator[](key);
    }
    JSON_ERROR_MSG(false, "invalid type use operator[], type = " << type_name());
}

BASIC_VALUE_TPL_DECL
void BASIC_VALUE_TPL::push_back(basic_value&& val) {
    if (is_null()) {
        *this = basic_value(value_t::array);
    }
    if (JSON_LIKELY(is_array())) {
        return value_.array->push_back(std::move(val));
    }
    JSON_ERROR_MSG(false, "invalid type use push_back, type = " << type_name());
}

// void push_back(const basic_value& val) {}

BASIC_VALUE_TPL_DECL
void BASIC_VALUE_TPL::push_back(typename object_t::value_type&& val) {
    if (is_null()) {
        *this = basic_value(value_t::object);
    }
    if (JSON_LIKELY(is_object())) {
        value_.object->insert(std::move(val));
        return;
    }
    JSON_ERROR_MSG(false, "invalid type use push_back, type = " << type_name());
}

BASIC_VALUE_TPL_DECL
void BASIC_VALUE_TPL::pop_back() {
    if (JSON_LIKELY(is_array())) {
        return value_.array->pop_back();
    }
    JSON_ERROR_MSG(false, "invalid type use pop_back(), type = " << type_name());
}

BASIC_VALUE_TPL_DECL
size_t BASIC_VALUE_TPL::size() const {
    switch (type_) {
        case value_t::null:
            return 0;
        case value_t::object:
            return value_.object->size();
        case value_t::array:
            return value_.array->size();
        default:
            break;
    }
    JSON_ERROR_MSG(false, "cannot use size() for this type, type = " << type_name());
}

BASIC_VALUE_TPL_DECL
bool BASIC_VALUE_TPL::empty() const {
    switch (type_) {
        case value_t::null:
            return true;
        case value_t::object:
            return value_.object->empty();
        case value_t::array:
            return value_.array->empty();
        default:
            break;
    }
    JSON_ERROR_MSG(false, "cannot use empty() for this type, type = " << type_name());
}


BASIC_VALUE_TPL_DECL
void BASIC_VALUE_TPL::reserve(size_t size) {
    switch (type_) {
        case value_t::null:
            *this = basic_value(value_t::array);
        case value_t::array:
            return value_.array->reserve(size);
        default:
            break;
    }
    JSON_ERROR_MSG(false, "cannot use reserve() for this type, type = " << type_name());
}

BASIC_VALUE_TPL_DECL
void BASIC_VALUE_TPL::resize(size_t size) {
    switch (type_) {
        case value_t::null:
            *this = basic_value(value_t::array);
        case value_t::array:
            return value_.array->resize(size);
        default:
            break;
    }
    JSON_ERROR_MSG(false, "cannot use resize() for this type, type = " << type_name());
}

BASIC_VALUE_TPL_DECL
void BASIC_VALUE_TPL::clear() noexcept {
    switch (type_) {
        case value_t::object:
            value_.object->clear();
            break;
        case value_t::array:
            value_.array->clear();
            break;
        case value_t::string:
            value_.string->clear();
            break;
        case value_t::number_int:
            value_.num_int = 0;
            break;
        case value_t::number_uint:
            value_.num_uint = 0;
            break;
        case value_t::number_real:
            value_.num_real = 0.0;
            break;
        case value_t::boolean:
            value_.boolean = false;
            break;
        default:
            break;
    }
}

BASIC_VALUE_TPL_DECL
void BASIC_VALUE_TPL::destory() {
    switch (type_) {
        case value_t::null:
            break;
        case value_t::object: {
            for (auto& [key, val] : *value_.object) {
                val.destory();
            }
            delete value_.object;
            value_.object = nullptr;
            break;
        }
        case value_t::array: {
            for (auto& val : *value_.array) {
                val.destory();
            }
            delete value_.array;
            value_.array = nullptr;
            break;
        }
        case value_t::boolean:
        case value_t::number_int:
        case value_t::number_uint:
        case value_t::number_real:
            break;
        case value_t::string:
            delete value_.string;
            break;
        default:
            break;
    }
    type_ = value_t::null;
}


BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::iterator BASIC_VALUE_TPL::begin() noexcept {
    iterator result(this);
    result.set_begin();
    return result;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::iterator BASIC_VALUE_TPL::end() noexcept {
    iterator result(this);
    result.set_end();
    return result;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_iterator BASIC_VALUE_TPL::begin() const noexcept {
    const_iterator result(this);
    result.set_begin();
    return result;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_iterator BASIC_VALUE_TPL::end() const noexcept {
    const_iterator result(this);
    result.set_end();
    return result;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_iterator BASIC_VALUE_TPL::cbegin() const noexcept {
    const_iterator result(this);
    result.set_begin();
    return result;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_iterator BASIC_VALUE_TPL::cend() const noexcept {
    const_iterator result(this);
    result.set_end();
    return result;
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::reverse_iterator BASIC_VALUE_TPL::rbegin() noexcept {
    return reverse_iterator(end());
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::reverse_iterator BASIC_VALUE_TPL::rend() noexcept {
    return reverse_iterator(begin());
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_reverse_iterator BASIC_VALUE_TPL::rbegin() const noexcept {
    return const_reverse_iterator(cend());
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_reverse_iterator BASIC_VALUE_TPL::rend() const noexcept {
    return const_reverse_iterator(cbegin());
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_reverse_iterator BASIC_VALUE_TPL::crbegin() const noexcept {
    return const_reverse_iterator(cend());
}

BASIC_VALUE_TPL_DECL
typename BASIC_VALUE_TPL::const_reverse_iterator BASIC_VALUE_TPL::crend() const noexcept {
    return const_reverse_iterator(cbegin());
}

BASIC_VALUE_TPL_DECL
template <typename T>
inline T BASIC_VALUE_TPL::as() {
    if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_same_v<T, boolean_t>) {
            return as_boolean();
        }
        if constexpr (std::is_signed_v<T>) {
            return as_int();
        } else {
            return as_uint();
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        return as_real();
    } else if constexpr (meta::is_string_v<T>) {
        return as_string();
    } else if constexpr (meta::is_cstring_v<T>) {
        return as_cstring();
    } else {
        JSON_ERROR_MSG(false, "invalid type use as()");
    }
}

BASIC_VALUE_TPL_DECL
template <typename T>
inline bool BASIC_VALUE_TPL::is() const noexcept {
    if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_same_v<T, boolean_t>) {
            return is_boolean();
        } 
        if constexpr (std::is_signed_v<T>) {
            return is_num_int();
        } else {
            return is_num_uint();
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        return is_real();
    } else if constexpr (meta::is_string_v<T>) {
        return is_string();
    } else if constexpr (meta::is_cstring_v<T>) {
        return is_string();
    } else {
        if constexpr (std::is_same_v<T, nullptr_t>) {
            return is_null();
        } else if constexpr (std::is_same_v<T, array_t>) {
            return is_array();
        } else if constexpr (std::is_same_v<T, object_t>) {
            return is_object();
        } else {
            // TODO write a log
            JSON_LOG("invalid type, can not use is");
            return false;
        }
    }
}

// using value = basic_value<>;
using value = basic_value<std::map, std::vector, std::string, bool, int, uint, float, std::allocator>;

#undef BASIC_VALUE_TPL
#undef BASIC_VALUE_TPL_DECL

}