#pragma once

#include <clocale>
#include "marco.h"

namespace json {

struct position_t {
    std::size_t chars_read_total = 0;               // the total number of characters read
    std::size_t chars_read_current_line = 0;        // the number of lines read
    std::size_t chars_read_offset_line = 0;         // the offset of lines read

    constexpr operator size_t() const {
        return chars_read_total;
    }
};

enum class token_type {
    literal_true,           //  `true`
    literal_false,          //  `false`
    literal_null,           //  `null`
    value_string,
    value_unsigned,
    value_integer,
    value_real,
    array_begin,            // `[`
    array_end,              // `]`
    object_begin,           // `{`
    object_end,             // `}`
    data_separator,         // `,`
    object_separator,       // `:`
    end_of_input,
    parse_error,
};


static const char* token_type_name(const token_type t) noexcept {
    switch (t) {
        case token_type::literal_true:
            return "true literal";
        case token_type::literal_false:
            return "false literal";
        case token_type::literal_null:
            return "null literal";
        case token_type::value_string:
            return "string literal";
        case token_type::value_unsigned:
        case token_type::value_integer:
        case token_type::value_real:
            return "number literal";
        case token_type::array_begin:
            return "'['";
        case token_type::array_end:
            return "']";
        case token_type::object_begin:
            return "'{'";
        case token_type::object_end:
            return "'}";
        case token_type::data_separator:
            return "','";
        case token_type::object_separator:
            return "':'";
        case token_type::parse_error:
            return "<parse error>";
        default:
            return "unknown token";
    }
}

template <typename BasicJsonType, typename InputAdapterType, bool ignore_comments = false>
class lexer {
private:
    using num_int_t = typename BasicJsonType::num_int_t;
    using num_uint_t = typename BasicJsonType::num_uint_t;
    using num_real_t = typename BasicJsonType::num_real_t;
    using string_t = typename BasicJsonType::string_t;
    using char_type = typename InputAdapterType::char_type;
    using char_int_type = typename std::char_traits<char_type>::int_type;
public:
    explicit lexer(InputAdapterType&& adapter) noexcept 
        : ia(std::forward<InputAdapterType>(adapter))
        , decimal_point_char(static_cast<char_int_type>(get_decimal_point()))  
    {}

    lexer(const lexer&) = delete;
    lexer(lexer&&) noexcept = default;
    lexer& operator=(const lexer&) = delete;
    lexer& operator=(lexer&&) = delete;
    ~lexer() = default;

protected:
    static char get_decimal_point() noexcept {
        const auto* loc = localeconv();
        JSON_ERROR(loc != nullptr);
        return (loc->decimal_point == nullptr) ? '.' : *(loc->decimal_point);
    }

    int get_codepoint() {
        int codepoint = 0;
        // for (int i = 0; i < 4; ++i) {
        for (const auto factor : {12, 8, 4, 0} ) {
            // codepoint *= 16;
            get();
            if (current >= '0' && current <= '9') {
                codepoint += ((current - 0x30) << factor);
                // codepoint += (current - '0');
            } else if (current >= 'A' && current <= 'F') {
                codepoint += ((current - 0x37) << factor);
                // codepoint += current - 'A' + 10;
            } else if (current >= 'a' && current <= 'f') {
                codepoint += ((current - 0x57) << factor);
                // codepoint += current - 'a' + 10;
            } else {
                return -1;
            }
        }
        return codepoint;
    }

    void codepoint_to_utf8(int codepoint) {
        if (codepoint <= 0x7f) {
            // 0xxxxxxx
            add(static_cast<char_int_type>(codepoint));
        } else if (codepoint <= 0x7FF) {
            // 110xxxxx 10xxxxxx
            add(static_cast<char_int_type>(0xC0 | (codepoint >> 6)));
            add(static_cast<char_int_type>(0x80 | (codepoint & 0x3f)));
        } else if (codepoint <= 0xFFFF) {
            // 1110xxxx 10xxxxx 10xxxxxx
            add(static_cast<char_int_type>(0xE0 | (codepoint >> 12)));
            add(static_cast<char_int_type>(0x80 | ((codepoint >> 6) & 0x3f)));
            add(static_cast<char_int_type>(0x80 | (codepoint & 0x3f)));
        } else {
            // 11110xxx 10xxxxx 10xxxxxx 10xxxxxx
            add(static_cast<char_int_type>(0xF0 | (codepoint >> 18)));
            add(static_cast<char_int_type>(0x80 | ((codepoint >> 12) & 0x3f)));
            add(static_cast<char_int_type>(0x80 | ((codepoint >> 6) & 0x3f)));
            add(static_cast<char_int_type>(0x80 | (codepoint & 0x3f)));
        }
    }

    void skip_whitespace() {
        do {
            get();
        } while (current == ' ' || current == '\t' || current == '\n' || current == '\r');
    }

    char_int_type get() {
        if (JSON_UNLIKELY(next_unget)) {
            next_unget = false;
        } else {
            current = ia.get_next_char();
        }
        
        return current;
    }

    void unget() {
        next_unget = true;
    }

    void add(char_int_type c) {
        token_buffer.push_back(static_cast<typename string_t::value_type>(c));
    }

    void reset() noexcept {
        token_buffer.clear();
    }

    bool scan_comment() {
        switch (get()) {
            case '/': {
                while (true) {
                    switch (get()) {
                        case '\n':
                        case '\r':
                        case std::char_traits<char_type>::eof():
                        case '\0':
                            return true;
                        default:
                            break;
                    }
                }
            }
            case '*': {
                while (true) {
                    switch (get()) {
                        case std::char_traits<char_type>::eof():
                        case '\0': {
                            error_message += "invalid comment; missing closing '*/'";
                            return false;
                        }
                        case '*': {
                            switch (get()) {
                                case '/':
                                    return true;
                                default: {
                                    unget();
                                    continue;
                                }
                            }
                        }
                        default:
                            continue;
                    }
                }
            }
            default: {
                error_message += "invalid comment; expecting '/' or '*' after '/'";
                return false;
            }
        }
    }

    token_type scan_string() {
        reset();
        JSON_ERROR(current == '\"');

        while (true) {
            switch (get()) {
                case std::char_traits<char_type>::eof(): {
                    error_message += "invalid string: missing closing quote\n";
                    return token_type::parse_error;
                }
                case '\"':
                    return token_type::value_string;
                case '\\': {
                    switch (get()) {
                        case '\"':
                            add('\"');
                            break;
                        case '\\':
                            add('\\');
                            break;
                        case '/':
                            add('/');
                            break;
                        case 'b':
                            add('\b');
                            break;
                        case 'f':   
                            add('\f');
                            break;
                        case 'n':   
                            add('\n');
                            break;
                        case 'r':
                            add('\r');
                            break;
                        case 't':
                            add('\t');
                            break;
                        case 'u': {
                            const int codepoint1 = get_codepoint();
                            int codepoint = codepoint1;

                            if (JSON_UNLIKELY(codepoint == -1)) {
                                error_message += "invalid string: '\\u' must be followed by 4 hex digits\n";
                                return token_type::parse_error;
                            }

                            // surrogate pair
                            if (codepoint1 >= 0xD800 && codepoint1 <= 0xDBFF) {
                                if (JSON_LIKELY(get() == '\\' && get() == 'u')) {
                                    const int codepoint2 = get_codepoint();

                                    if (JSON_UNLIKELY(codepoint2 == -1)) {
                                        error_message += "invalid string: '\\u' must be followed by 4 hex digits\n";
                                        return token_type::parse_error;
                                    }
                                    // codepoint = 0x10000 + (codepoint1 − 0xD800) × 0x400 + (codepoint2 − 0xDC00)
                                    //           = 0x10000 + (codepoint1 << 10) - 0x3600000 - 0xDC00 + codepoint2
                                    //           = (codepoint1 << 10) + codepoint2 - 0x35fdc00
                                    if (JSON_LIKELY(codepoint2 >= 0xDC00 && codepoint2 <= 0xDFFF)) {
                                        codepoint = (codepoint1 << 10) + codepoint2 - 0x35fdc00;
                                    }
                                }
                            } else {
                                if (JSON_UNLIKELY(codepoint1 >= 0xDC00 && codepoint1 <= 0xDFFF)) {
                                    error_message += "invalid string: surrogate U+DC00..U+DFFF must follow U+D800..U+DBFF\n";
                                    return token_type::parse_error;
                                }
                            }

                            JSON_ERROR(codepoint >= 0x00 && codepoint <= 0x10FFFF);

                            if constexpr (sizeof(typename string_t::value_type) > 1) {
                                add(codepoint);
                                break;
                            }
                            codepoint_to_utf8(codepoint);
                            break;
                        }
                        default:
                            error_message += "invalid string: forbidden character after backslash\n";
                            return token_type::parse_error;
                    }
                    break;
                }
                default:
                    add(current);
                    break;
            }
        }
    }

/*
    state    | 0        | 1-9      | e E      | +       | -       | .        | anything
    ---------|----------|----------|----------|---------|---------|----------|-----------
    init     | zero     | any1     | [error]  | [error] | minus   | [error]  | [error]
    minus    | zero     | any1     | [error]  | [error] | [error] | [error]  | [error]
    zero     | done     | done     | exponent | done    | done    | decimal1 | done
    any1     | any1     | any1     | exponent | done    | done    | decimal1 | done
    decimal1 | decimal2 | decimal2 | [error]  | [error] | [error] | [error]  | [error]
    decimal2 | decimal2 | decimal2 | exponent | done    | done    | done     | done
    exponent | any2     | any2     | [error]  | sign    | sign    | [error]  | [error]
    sign     | any2     | any2     | [error]  | [error] | [error] | [error]  | [error]
    any2     | any2     | any2     | done     | done    | done    | done     | done
*/
    token_type scan_number() {
        reset();
        token_type number_type = token_type::value_unsigned;
// scan_number_init:
        switch (current) {
            case '-': {
                add(current);
                goto scan_number_minus;
            }
            case '0':
                add(current);
                goto scan_number_zero;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                add(current);
                goto scan_number_any1;
            }
            default:
                JSON_ERROR_MSG(false, "never goto there");
                break;
        }
scan_number_minus:
        number_type = token_type::value_integer;
        switch (get()) {
            case '0': {
                add(current);
                goto scan_number_zero;
            }   
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                add(current);
                goto scan_number_any1;
            }
            default:
                error_message += "invalid number; expected digit after '-'\n";
                return token_type::parse_error;
        }

scan_number_zero:
        switch (get()) {
            case '.': {
                add(decimal_point_char);
                goto scan_number_decimal1;
            }
            case 'e':
            case 'E': {
                add(current);
                goto scan_number_exponent;
            }
            default:
                goto scan_number_done;
        }

scan_number_any1:
        switch (get()) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                add(current);
                goto scan_number_any1;
            }
            case '.': {
                add(decimal_point_char);
                goto scan_number_decimal1;
            }
            case 'e':
            case 'E': {
                add(current);
                goto scan_number_exponent;
            }
            default:
                goto scan_number_done;
        }
scan_number_decimal1:
        number_type = token_type::value_real;
        switch (get()) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                add(current);
                goto scan_number_decimal2;
            }
            default:
                error_message += "invalid number; expected digit after '.'\n";
                return token_type::parse_error;
        }
scan_number_decimal2:
        switch (get()) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                add(current);
                goto scan_number_decimal2;
            }
            case 'e':
            case 'E': {
                add(current);
                goto scan_number_exponent;
            }
            default:
                goto scan_number_done;
        }

scan_number_exponent:
    number_type = token_type::value_real;
    switch (get()) {
        case '+':
        case '-': {
            add(current);
            goto scan_number_sign;
        }        
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            add(current);
            goto scan_number_any2;
        }
        default:
            error_message += "invalid number; expected '+', '-', or digit after exponent\n";
            return token_type::parse_error;
    }

scan_number_sign:
    switch (get()) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            add(current);
            goto scan_number_any2;
        }
        default:
            error_message += "invalid number; expected digit after exponent sign\n";
            return token_type::parse_error;     
    }

scan_number_any2:
    switch (get()) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            add(current);
            goto scan_number_any2;
        } 
        default:
            goto scan_number_done;
    }

scan_number_done:
        unget();
        return number_type;
    }



    token_type scan() {
        skip_whitespace();

        if constexpr (ignore_comments) {
            while (current == '/') {
                if (!scan_comment()) {
                    return token_type::parse_error;
                }
                skip_whitespace();
            }
        }

        switch (current) {
            case '[':
                return token_type::array_begin;
            case ']':
                return token_type::array_end;
            case '{':
                return token_type::object_begin;
            case '}':
                return token_type::object_end;
            case ':':
                return token_type::object_separator;
            case ',':
                return token_type::data_separator;
            case 't':
                return scan_literal("true", 4, token_type::literal_true);
            case 'f':
                return scan_literal("false", 5, token_type::literal_false);
            case 'n':
                return scan_literal("null", 4, token_type::literal_null);
            case '\"':
                return scan_string();
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return scan_number();
            case '\0':
            case std::char_traits<char_type>::eof():
                return token_type::end_of_input;
            default:
                error_message += "invalid literal\n";
                return token_type::parse_error;
        }
    }

    token_type scan_literal(const char* ptr, std::size_t length, token_type return_type) {
        for (std::size_t i = 1; i < length; ++i) {
            if (JSON_UNLIKELY(std::char_traits<char_type>::to_char_type(get()) != ptr[i])) {
                error_message += "invalid literal\n";
                return token_type::parse_error;
            }
        }
        return return_type;
    }

    num_int_t get_number_integer() const noexcept {
        if constexpr (sizeof(typename string_t::value_type) == 1) {
            auto* begin = (const char*)token_buffer.data();
            char* end = nullptr;
            if constexpr (sizeof(num_int_t) <= 4) {
                return atoi(begin);
            } else if constexpr (sizeof(num_int_t) <= 8) {
                return strtoll(begin, &end, 10);
            } else {
                return strtoll(begin, &end, 10);
            }
        } else {
            std::basic_stringstream<typename string_t::value_type> ss;
            ss << token_buffer;
            num_int_t result;
            ss >> result;
            return result;
            error_message += "string_t::value_type sizeof is not 1, cannot convert string to integer\n";
        }
        return 0;
    }

    num_uint_t get_number_unsigned() const noexcept {
        if constexpr (sizeof(typename string_t::value_type) == 1) {
            auto* begin = (const char*)token_buffer.data();
            char* end = nullptr;
            if constexpr (sizeof(num_uint_t) <= 8) {
                return strtoul(begin, &end, 10);
            } else {
                return strtoull(begin, &end, 10);
            } 
        } else {
            std::basic_stringstream<typename string_t::value_type> ss;
            ss << token_buffer;
            num_uint_t result;
            ss >> result;
            return result;
            error_message += "string_t::value_type sizeof is not 1, cannot convert string to unsigned integer\n";
        }
        return 0;
    }

    num_real_t get_number_real() const noexcept {
        if constexpr (sizeof(typename string_t::value_type) == 1) {
            auto* begin = (const char*)token_buffer.data();
            char* end = nullptr;
            if constexpr (sizeof(num_real_t) <= 4) {
                return strtof(begin, &end);
            } else if constexpr (sizeof(num_real_t) <= 8) {
                return strtod(begin, &end);
            } else {
                return strtold(begin, &end);
            } 
        } else {
            std::basic_stringstream<typename string_t::value_type> ss;
            ss << token_buffer;
            num_real_t result;
            ss >> result;
            return result;
            error_message += "string_t::value_type sizeof is not 1, cannot convert string to float\n";
        }
        return 0; 
    }

    string_t& get_string() {
        return token_buffer;
    }

protected:
    InputAdapterType ia;

    char_int_type decimal_point_char = '.';

    char_int_type current = std::char_traits<char_type>::eof();

    mutable std::string error_message = "";
    string_t token_buffer;

    bool next_unget = false;
};

} // namespace json