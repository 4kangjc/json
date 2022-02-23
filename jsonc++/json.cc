#include "json.h"
#include <iostream>
#include <exception>

namespace json {

basic_value::json_value::json_value(value_t type) {
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

basic_value::basic_value(value_t type) 
    : type_(type), value_(type) {

}

basic_value::basic_value(boolean_t v) 
    : type_(value_t::boolean), value_(v) {
}

basic_value::basic_value(num_int_t v) 
    : type_(value_t::number_int), value_(v) {

}

basic_value::basic_value(num_uint_t v) 
    : type_(value_t::number_uint), value_(v) {

}

basic_value::basic_value(const string_t& v) 
    : type_(value_t::string), value_(v) {

}

basic_value::basic_value(string_t&& v) 
    : type_(value_t::string), value_(v) {

}

basic_value::basic_value(num_real_t v) 
    : type_(value_t::number_real), value_(v) {

}

basic_value::basic_value(basic_value&& rhs) {
    destory();
    value_ = rhs.value_;
    type_ = rhs.type_;
    rhs.value_ = {};
    rhs.type_ = value_t::null;
}

basic_value& basic_value::operator=(basic_value&& rhs) {
    if (this != &rhs) {
        destory();
        value_ = rhs.value_;
        type_ = rhs.type_;
        rhs.value_ = {};
        rhs.type_ = value_t::null;
    }
    return *this;
}

const char* basic_value::type_name() const noexcept {
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

bool basic_value::as_boolean() const {
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
    JSON_ERROR_MSG(false, "value cant't convertible to bool, type = " << type_name());
    return false;
}

const char* basic_value::as_cstring() const {
    if (type_ == value_t::string) {
        return value_.string->c_str();
    }
    JSON_ERROR_MSG(false, "value_type is not string, type = " << type_name());
    return nullptr;
}

basic_value::string_t basic_value::as_string() const {
    switch (type_) {
        case value_t::null:
            return "null";
        case value_t::boolean:
            return value_.boolean ? "true" : "false";
        case value_t::number_int:
            return std::to_string(value_.num_int);
        case value_t::number_uint:
            return std::to_string(value_.num_uint);
        case value_t::number_real:
            return std::to_string(value_.num_real);
        case value_t::string:
            return *value_.string;
        default:
            break;
    }
    JSON_ERROR_MSG(false, "value cant't convertible to string, type = " << type_name());
    return "";
}

float basic_value::as_float() const {
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
    JSON_ERROR_MSG(false, "value cant't convertible to float, type = " << type_name());
    return 0;
}

double basic_value::as_double() const {
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
    JSON_ERROR_MSG(false, "value cant't convertible to double, type = " << type_name());
    return 0;
}

basic_value::num_int_t basic_value::as_int() const {
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
    JSON_ERROR_MSG(false, "value cant't convertible to int, type = " << type_name());
}

basic_value::num_uint_t basic_value::as_uint() const {
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
    JSON_ERROR_MSG(false, "value cant't convertible to uint, type = " << type_name());
}

basic_value::reference basic_value::operator[](size_t idx) {
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

// static const basic_value null;

basic_value::const_reference basic_value::operator[](size_t idx) const {
    if (JSON_LIKELY(is_array())) {
        if (value_.array->size() <= idx) {
            JSON_ERROR_MSG(false, "out of range, array size = " << value_.array->size()
            << ", index = " << idx);
        }
        return value_.array->operator[](idx);
    }
    JSON_ERROR_MSG(false, "invalid type use operator[], type = " << type_name());
}

basic_value::reference basic_value::operator[](const object_t::key_type& key) {
    switch (type_) {
        case value_t::null:
            *this = basic_value(value_t::object);
        case value_t::object:
            return value_.object->operator[](key);
        default:
            JSON_ERROR_MSG(false, "invalid type use operator[], type = " << type_name());
    }
}

basic_value::const_reference basic_value::operator[](const object_t::key_type& key) const {
    if (JSON_LIKELY(is_object())) {
        return value_.object->operator[](key);
    }
    JSON_ERROR_MSG(false, "invalid type use operator[], type = " << type_name());
}

void basic_value::push_back(basic_value&& val) {
    if (is_null()) {
        *this = basic_value(value_t::array);
    }
    if (JSON_LIKELY(is_array())) {
        return value_.array->push_back(std::move(val));
    }
    JSON_ERROR_MSG(false, "invalid type use push_back, type = " << type_name());
}

// void push_back(const basic_value& val) {}

size_t basic_value::size() const {
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

bool basic_value::empty() const {
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


void basic_value::reserve(size_t size) {
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

void basic_value::resize(size_t size) {
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

void basic_value::clear() noexcept {
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

void basic_value::destory() {
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


basic_value::iterator basic_value::begin() noexcept {
    iterator result(this);
    result.set_begin();
    return result;
}

basic_value::iterator basic_value::end() noexcept {
    iterator result(this);
    result.set_end();
    return result;
}

basic_value::const_iterator basic_value::cbegin() const noexcept {
    const_iterator result(this);
    result.set_begin();
    return result;
}

basic_value::const_iterator basic_value::cend() const noexcept {
    const_iterator result(this);
    result.set_end();
    return result;
}

basic_value::reverse_iterator basic_value::rbegin() noexcept {
    return reverse_iterator(end());
}

basic_value::reverse_iterator basic_value::rend() noexcept {
    return reverse_iterator(begin());
}

basic_value::const_reverse_iterator basic_value::rbegin() const noexcept {
    return const_reverse_iterator(cend());
}

basic_value::const_reverse_iterator basic_value::rend() const noexcept {
    return const_reverse_iterator(cbegin());
}


basic_value::const_reverse_iterator basic_value::crbegin() const noexcept {
    return const_reverse_iterator(cend());
}

basic_value::const_reverse_iterator basic_value::crend() const noexcept {
    return const_reverse_iterator(cbegin());
}




} // namespace json