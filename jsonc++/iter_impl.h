#pragma once

#include <iterator>

namespace json::iter {

template <typename BasicJsonType>
class impl {
public:
    using object_t = BasicJsonType::object_t;
    using array_t = BasicJsonType::array_t;
    
    using value_type = BasicJsonType::value_type;

private:
    
};

template <typename iter>
class reverse_iterator : public std::reverse_iterator<iter> {
public:

};

}