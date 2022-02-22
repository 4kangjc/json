#pragma once

#include <iterator>
#include <variant>
#include <iostream>
#include <cassert>

#include "value_t.h"
#include "marco.h"

namespace json::iter {

template <typename BasicJsonType>
class impl {
public:
    using other_impl = impl<std::conditional_t<std::is_const_v<BasicJsonType>,
                        std::remove_const_t<BasicJsonType>, const BasicJsonType>>;

    // friend BasicJsonType;
    // friend other_impl;

    using object_t = typename BasicJsonType::object_t;
    using array_t = typename BasicJsonType::array_t;
    
    using value_type = typename BasicJsonType::value_type;
    using pointer = std::conditional_t<std::is_const_v<BasicJsonType>, 
                                        typename BasicJsonType::const_pointer, 
                                        typename BasicJsonType::pointer>;
    using reference = std::conditional_t<std::is_const_v<BasicJsonType>,
                                        typename BasicJsonType::reference,
                                        typename BasicJsonType::const_reference>;


    impl() = default;
    ~impl() = default;
    impl(impl&& ) noexcept = default;
    impl& operator=(impl&& ) noexcept = default;


    explicit impl(pointer object) noexcept : obj_(object) {
        JSON_ERROR(obj_ != nullptr);
        switch (obj_->type_) {
            case value_t::object:
                iter_ = typename object_t::iterator();
                break;
            case value_t::array:
                iter_ = typename array_t::iterator();
                break;
            default:
                break; 
        }
    }

    reference operator*() const {
        JSON_ERROR(obj_ != nullptr);
        switch (obj_->type_) {
            case value_t::object: {
                auto& iter = *std::get_if<0>(&iter_);
                JSON_ERROR(iter != obj_->value_.object->end());
                return iter->second;
            }
            case value_t::array: {
                auto& iter = *std::get_if<1>(&iter_);
                JSON_ERROR(iter != obj_->value_.array->end());
                return *iter;
            }
            default:
                return *obj_;
        }
    }

    pointer operator->() const {
        JSON_ERROR(obj_ != nullptr);
        switch (obj_->type_) {
            case value_t::object: {
                auto& iter = *std::get_if<0>(&iter_);
                JSON_ERROR(iter != obj_->value_.object->end());
                return &iter->second;
            }
            case value_t::array: {
                auto& iter = *std::get_if<1>(&iter_);
                JSON_ERROR(iter != obj_->value_.array->end());
                return &*iter;
            }
            default:
                return obj_;
        }
    }

    impl& operator++() {
        JSON_ERROR(obj_ != nullptr);
        switch (obj_->type_) {
            case value_t::object:
                std::advance(std::get<0>(iter_), 1);
                break;
            case value_t::array:
                std::advance(std::get<1>(iter_), 1);
                break;
            default:
                JSON_ERROR_MSG(false, "cannot use operator++, iterator type is not object or array");
        }
        return *this;
    }

    impl& operator--() {
        JSON_ERROR(obj_ != nullptr);
        switch (obj_->type_) {
            case value_t::object:
                std::advance(std::get<0>(iter_), -1);
                break;
            case value_t::array:
                std::advance(std::get<1>(iter_), -1);
                break;
            default:
                JSON_ERROR_MSG(false, "cannot use operator--, iterator type is not object or array");
        }
    }

    template <typename Iter, typename = 
        std::enable_if_t<(std::is_same_v<Iter, impl> || std::is_same_v<Iter, other_impl>)>>
    bool operator==(const Iter& rhs) const {
        JSON_ERROR(iter_.index() == rhs.iter_.index());        // 迭代器类型相同 都是 array 或者都是 object
        JSON_ERROR(obj_ != nullptr);

        switch (obj_->type_) {
            case value_t::object:
            case value_t::array:
                JSON_ERROR(obj_ == rhs.obj_);
                return iter_ == rhs.iter_;
            default:
                return obj_ == rhs.obj_;
        }
    }

    template <typename Iter, typename = 
        std::enable_if_t<(std::is_same_v<Iter, impl> || std::is_same_v<Iter, other_impl>)>>
    bool operator!=(const Iter& rhs) const {
        return !(*this == rhs);
    }

    const typename object_t::key_type& key() const {
        JSON_ERROR(obj_ != nullptr);
        if (JSON_LIKELY(obj_->is_object())) {
            return std::get<0>(iter_)->first;
        }
        JSON_ERROR_MSG(false, "cannot use key() for non-object iterators");
    }


    reference value() const {
        return this->operator*();
    }

    void set_begin() noexcept {
        JSON_ERROR(obj_ != nullptr);
        switch (obj_->type_) {
            case value_t::object: {
                auto& iter = *std::get_if<0>(&iter_);
                iter = obj_->value_.object->begin();
                break;
            }
            case value_t::array: {
                auto& iter = *std::get_if<1>(&iter_);
                iter = obj_->value_.array->begin();
                break;
            }
            default:
                JSON_ERROR_MSG(false, "cannot use set_begin() for non-object or non-array iterators");
                break;
        }
    }
    void set_end() noexcept {
        JSON_ERROR(obj_ != nullptr);
        switch (obj_->type_) {
            case value_t::object: {
                auto& iter = *std::get_if<0>(&iter_);
                iter = obj_->value_.object->end();
                break;
            }
            case value_t::array: {
                auto& iter = *std::get_if<1>(&iter_);
                iter = obj_->value_.array->end();
                break;
            }
            default:
                JSON_ERROR_MSG(false, "cannot use set_begin() for non-object or non-array iterators");
                break;
        }
    }
private:
    std::variant<typename object_t::iterator, typename array_t::iterator> iter_{};
    pointer obj_ = nullptr;
};

template <typename iter>
class reverse_iterator : public std::reverse_iterator<iter> {
public:

};

} // namespace json::iter