#pragma once

#include <vector>
#include "noncopyable.h"

namespace json {

template <typename BasicJsonType>
class json_sax_dom_parser : noncopyable {
public:
    using num_int_t = typename BasicJsonType::num_int_t;
    using num_uint_t = typename BasicJsonType::num_uint_t;
    using string_t = typename BasicJsonType::string_t;
public:
    explicit json_sax_dom_parser(BasicJsonType& result)
        : root(result)
    {}

    ~json_sax_dom_parser() = default;

    void start_object() {
        nodes.push_back(handle_value(value_t::object));
    }

    void end_object() {
        nodes.pop_back();
    }

    void start_array() {
        nodes.push_back(handle_value(value_t::array));
    }

    void end_array() {
        nodes.pop_back();
    }

    bool key(string_t& val) {
        if (JSON_UNLIKELY(nodes.empty())) {
            return false;
        }
        object_element = &(nodes.back()->value_.object->operator[](val));
        return true;
    }

    bool key(string_t&& val) {
        if (JSON_UNLIKELY(nodes.empty())) {
            return false;
        }
        auto [it, ok] = nodes.back()->value_.object->try_emplace(std::move(val));
        // auto [it, ok] = nodes.back()->value_.object->emplace(std::move(val), nullptr);
        if (JSON_UNLIKELY(!ok)) {
            return false;
        }
        object_element = &it->second;
        // object_element = &(nodes.back()->value_.object->operator[](std::move(val)));
        return true;
    }

    template <typename Value>
    BasicJsonType* handle_value(Value&& v) {
        if (nodes.empty()) {
            root = BasicJsonType(std::forward<Value>(v));
            return &root;
        }
        
        switch (nodes.back()->type()) {
            case value_t::array: {
                nodes.back()->value_.array->emplace_back(std::forward<Value>(v));
                return &(nodes.back()->value_.array->back());
            }
            case value_t::object: {
                JSON_ERROR(object_element);
                *object_element = std::forward<Value>(v);
                return object_element;
            }
            default:
                return nullptr;
        }
    }
private:
    BasicJsonType& root;
    BasicJsonType* object_element;
    std::vector<BasicJsonType*> nodes;
};
    
} // namespace json
