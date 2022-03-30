#pragma once

#include "meta.h"
#include <memory>

namespace json {

template <typename CharType> 
struct output_adapter_interface {
    virtual void write_character(CharType c) = 0;
    virtual void write_characters(const CharType* s, std::size_t length) = 0;
    virtual ~output_adapter_interface() = default;

    output_adapter_interface() = default;
    output_adapter_interface(const output_adapter_interface&) = default;
    output_adapter_interface(output_adapter_interface&&) = default;
    output_adapter_interface& operator=(const output_adapter_interface&) = default;
    output_adapter_interface& operator=(output_adapter_interface&&) = default;
};

template <typename CharType>
using output_adapter_t = std::shared_ptr<output_adapter_interface<CharType>>;

template <typename CharType, typename = std::enable_if_t<meta::is_char_type_v<CharType>>>
class vector_output_adapter : public output_adapter_interface<CharType> {
public:
    explicit vector_output_adapter(std::vector<CharType>& vec) noexcept 
        : vector(vec) 
    {}

    void write_character(CharType c) override {
        vector.push_back(c);
    }

    void write_characters(const CharType* s, std::size_t length) override {
        std::copy(s, s + length, std::back_inserter(vector));
    }
private:
    std::vector<CharType>& vector;
};

template <typename CharType>
class stream_output_adapter : public output_adapter_interface<CharType> {
public:
    explicit stream_output_adapter(std::basic_ostream<CharType>& os) noexcept
        : stream(os) 
    {}

    void write_character(CharType c) override {
        stream.put(c);
    }

    void write_characters(const CharType* s, std::size_t length) override {
        stream.write(s, static_cast<std::streamsize>(length));
    } 
private:
    std::basic_ostream<CharType>& stream;
};


template <typename CharType, typename StringType = std::basic_string<CharType>>
class string_output_adapter : public output_adapter_interface<CharType> {
public:
    explicit string_output_adapter(StringType& s) noexcept 
        : string(s) 
    {}

    void write_character(CharType c) override {
        string.push_back(c);
    }

    void write_characters(const CharType* s, std::size_t length) override {
        string.append(s, length);
    }    
private:
    StringType& string;
};

template <typename CharType>
class output_adapter {
public:
    output_adapter(std::vector<CharType>& vec) 
        : oa(std::make_shared<vector_output_adapter<CharType>>(vec))
    {}

    output_adapter(std::basic_ostream<CharType>& os) 
        : oa(std::make_shared<stream_output_adapter<CharType>>(os))
    {}

    output_adapter(std::basic_string<CharType>& s) 
        : oa(std::make_shared<string_output_adapter<CharType>>(s))
    {}

    const auto& operator->() const {
        return oa;
    }
private:
    output_adapter_t<CharType> oa = nullptr;
};

} // namespace json