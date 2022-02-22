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
            std::cout << "invalid type" << std::endl;
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
    std::cout << "value cant't convertible to bool, type = " << type_name() << std::endl;
    return false;
}

const char* basic_value::as_cstring() const {
    if (type_ == value_t::string) {
        return value_.string->c_str();
    }
    std::cout << "value_type is not string, type = " << type_name() << std::endl;
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
    std::cout << "value cant't convertible to string, type = " << type_name() << std::endl;
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
    std::cout << "value cant't convertible to float, type = " << type_name() << std::endl;
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
    std::cout << "value cant't convertible to double, type = " << type_name() << std::endl;
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
    std::cout << "value cant't convertible to int, type = " << type_name() << std::endl;
    return 0;
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
    std::cout << "value cant't convertible to uint, type = " << type_name() << std::endl;
    return 0;
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
            std::cout << "invalid type, type = " << type_name() << std::endl;
            throw std::exception();
    }
}

static const basic_value null;

basic_value::const_reference basic_value::operator[](size_t idx) const {
    if (is_array()) {
        if (value_.array->size() <= idx) {
            std::cout << "out of range, array size = " << value_.array->size()
            << ", index = " << idx << std::endl;
            return null;
        }
        return value_.array->operator[](idx);
    }
    std::cout << "invalid type, type = " << type_name() << std::endl;
    return null;
}

basic_value::reference basic_value::operator[](const object_t::key_type& key) {
    switch (type_) {
        case value_t::null:
            *this = basic_value(value_t::object);
        case value_t::object:
            return value_.object->operator[](key);
        default:
            std::cout << "invalid type, type = " << type_name() << std::endl;
            throw std::exception();
    }
}

basic_value::const_reference basic_value::operator[](const object_t::key_type& key) const {
    if (is_object()) {
        return value_.object->operator[](key);
    }
    std::cout << "invalid type, type = " << type_name() << std::endl;
    return null;
}

void basic_value::push_back(basic_value&& val) {
    if (is_null()) {
        *this = basic_value(value_t::array);
    }
    if (is_array()) {
        return value_.array->push_back(std::move(val));
    }
    std::cout << "invalid type, type = " << type_name() << std::endl;
}

// void push_back(const basic_value& val) {}


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

} // namespace json