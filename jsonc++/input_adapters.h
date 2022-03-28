#pragma once

#include <cstdio>
#include <cstring>
#include <type_traits>
#include <istream>
#include "noncopyable.h"
#include "marco.h"

namespace json {

class file_input_adapter : noncopyable {
public:
    using char_type = char;
    explicit file_input_adapter(std::FILE* f) noexcept 
        : file_(f) 
    {}

    file_input_adapter(file_input_adapter&& rhs) noexcept 
        : file_(rhs.file_) { rhs.file_ = nullptr; }
    file_input_adapter& operator=(file_input_adapter&&) = delete;
    ~file_input_adapter() = default;

    std::char_traits<char>::int_type get_next_char() {
        return std::fgetc(file_);
    }

private:
    std::FILE* file_;
};

class stream_input_adapter : noncopyable {
public:
    using char_type = char;

    ~stream_input_adapter() {
        if (is != nullptr) {
            is->clear(is->rdstate() & std::ios::eofbit);
        }
    }

    explicit stream_input_adapter(std::istream& i) 
        : is(&i), sb(i.rdbuf())
    {}

    stream_input_adapter(stream_input_adapter&& rhs) noexcept 
        : is(rhs.is), sb(rhs.sb) {
        rhs.is = nullptr;
        rhs.sb = nullptr;
    }

    std::char_traits<char>::int_type get_next_char() {
        auto res = sb->sbumpc();
        if (JSON_UNLIKELY(res == std::char_traits<char>::eof())) {
            is->clear(is->rdstate() | std::ios::eofbit);
        }
        return res;
    }

private:
    std::istream* is = nullptr;
    std::streambuf* sb = nullptr;
};

template <typename IteratorType>
class iterator_input_adapter {
public:
    using char_type = typename std::iterator_traits<IteratorType>::value_type;

    iterator_input_adapter(IteratorType&& begin, IteratorType&& end) 
        : current(std::move(begin)), last(std::move(end)) 
    {}

    iterator_input_adapter(const IteratorType& begin, const IteratorType& end) 
        : current(begin), last(end)
    {}

    typename std::char_traits<char_type>::int_type get_next_char() {
        if (JSON_UNLIKELY(current != last)) {
            auto result = std::char_traits<char_type>::to_int_type(*current);
            std::advance(current, 1);
            return result;
        }
        return std::char_traits<char_type>::eof();
    }
private:
    IteratorType current;
    IteratorType last;
};
    
inline file_input_adapter input_adapter(std::FILE* file) {
    return file_input_adapter(file);
}

inline stream_input_adapter input_adapter(std::istream& stream) {
    return stream_input_adapter(stream);
}

inline stream_input_adapter input_adapter(std::istream&& stream) {
    return stream_input_adapter(stream);
}

template <typename IteratorType>
inline iterator_input_adapter<IteratorType> input_adapter(IteratorType begin, IteratorType end) {
    return iterator_input_adapter(std::move(begin), std::move(end));
}

inline auto input_adapter(const std::string& s) {
    return input_adapter(s.begin(), s.end());
}

inline auto input_adapter(std::string_view s) {
    return input_adapter(s.begin(), s.end());
}

inline auto input_adapter(const char* s) {
    auto length = std::strlen(s);
    return input_adapter(s, s + length);
}

template <typename T, std::size_t N>
inline auto input_adapter(T (&array)[N]) {
    return input_adapter(array, array + N);
}

} // namespace json
