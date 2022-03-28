#pragma once

#include <functional>
#include <stdint.h>
#include <vector>
#include "lexer.h"
#include "json_sax.h"

namespace json {

enum class parse_event_t : std::uint8_t {
    /// the parser read `{` and started to process a JSON object
    object_start,
    /// the parser read `}` and finished processing a JSON object
    object_end,
    /// the parser read `[` and started to process a JSON array
    array_start,
    /// the parser read `]` and finished processing a JSON array
    array_end,
    /// the parser read a key of a value in an object
    key,
    /// the parser finished reading a JSON value
    value
};

template<typename BasicJsonType>
using parser_callback_t =
    std::function<bool(int /*depth*/, parse_event_t /*event*/, BasicJsonType& /*parsed*/)>;

template <typename BasicJsonType, typename InputAdapterType, bool ignore_comments = false>
class reader : public lexer<BasicJsonType, InputAdapterType, ignore_comments> {
public:
    using num_int_t = typename BasicJsonType::num_int_t;
    using num_uint_t = typename BasicJsonType::num_uint_t;
    using string_t = typename BasicJsonType::string_t;
    using lexer_t = lexer<BasicJsonType, InputAdapterType, ignore_comments>;
public:
    explicit reader(InputAdapterType&& adapter, const parser_callback_t<BasicJsonType>& cb = nullptr) 
        : lexer_t(std::move(adapter)), callback_(cb)
    {
        get_token();
    }

    explicit reader(InputAdapterType&& adapter, parser_callback_t<BasicJsonType>&& cb = nullptr) 
        : lexer_t(std::move(adapter)), callback_(std::move(cb))
    {
        get_token();
    }

    token_type get_token() {
        return last_token = lexer_t::scan();
    }

    bool parse(BasicJsonType& result) {
        if (callback_) {
            return false;
        } else {
            json_sax_dom_parser sdp(result);
            bool ok = sax_parse_internal(&sdp);
            if (!ok) {
                return false;
            }
            if (get_token() != token_type::end_of_input) {
                lexer_t::error_message += "error token type, is not end of input\n";
                return false;
            }
            return true;
        }
    }

    const std::string& get_error_msg() const {
        return lexer_t::error_message;
    }
private:
    template <typename SAX>
    bool sax_parse_internal(SAX* sax) {
        // array -> true object -> false
        std::vector<bool> states;   
        bool skip_to_state_evaluation = false;

        while (true) {
            if (!skip_to_state_evaluation) {
                switch (last_token) {
                    case token_type::object_begin: {
                        sax->start_object();

                        if (get_token() == token_type::object_end) {
                            sax->end_object();
                            break;
                        }

                        // parser key
                        if (JSON_UNLIKELY(last_token != token_type::value_string)) {
                            lexer_t::error_message += "invalid character after {, is not object key\n";
                            return false;
                        }

                        if (JSON_UNLIKELY(!sax->key(std::move(lexer_t::get_string())))) {
                            lexer_t::error_message += "nodes is empty\n";
                            return false;
                        }

                        // parser separator (:)
                        if (JSON_UNLIKELY(get_token() != token_type::object_separator)) {
                            lexer_t::error_message += "invalid character after object key, is not object separator ':'\n";
                            return false;
                        }
                        
                        states.push_back(false);

                        get_token();
                        continue;
                    }
                    case token_type::array_begin: {
                        sax->start_array();

                        if (get_token() == token_type::array_end) {
                            sax->end_array();
                            break;
                        }

                        states.push_back(true);
                        continue;
                    }
                    case token_type::value_real: {
                        if (JSON_UNLIKELY(sax->handle_value(lexer_t::get_number_real()) == nullptr)) {
                            lexer_t::error_message += "handle value error, invalid real value\n";
                            return false; 
                        } 
                        break;
                    }
                    case token_type::literal_false: {
                        if (JSON_UNLIKELY(sax->handle_value(false) == nullptr)) {
                            lexer_t::error_message += "handle value error, invalid false literal\n";
                            return false;
                        }
                        break;
                    }
                    case token_type::literal_true: {
                        if (JSON_UNLIKELY(sax->handle_value(true) == nullptr)) {
                            lexer_t::error_message += "handle value error, invalid true literal\n";
                            return false;
                        }
                        break;
                    }
                    case token_type::literal_null: {
                        if (JSON_UNLIKELY(sax->handle_value(nullptr) == nullptr)) {
                            lexer_t::error_message += "handle value error, invalid null literal\n";
                            return false;
                        }
                        break;
                    }
                    case token_type::value_integer: {
                        if (JSON_UNLIKELY(sax->handle_value(lexer_t::get_number_integer()) == nullptr)) {
                            lexer_t::error_message += "handle value error, invalid integer value\n";
                            return false;
                        }
                        break;
                    }
                    case token_type::value_unsigned: {
                        if (JSON_UNLIKELY(sax->handle_value(lexer_t::get_number_unsigned()) == nullptr)) {
                            lexer_t::error_message += "handle value error, invalid unsigned value\n";
                            return false;
                        }
                        break;
                    }
                    case token_type::value_string: {
                        if (JSON_UNLIKELY(sax->handle_value(std::move(lexer_t::get_string())) == nullptr)) {
                            lexer_t::error_message += "handle value error, invalid string value\n";
                            return false;
                        }
                        break;
                    }
                    case token_type::parse_error: {
                        return false;
                    }
                    case token_type::array_end:
                    case token_type::object_end:
                    case token_type::data_separator:
                    case token_type::object_separator:
                    case token_type::end_of_input:
                    default: {
                        lexer_t::error_message += "invalid token_type, token_type = ";
                        lexer_t::error_message += token_type_name(last_token);
                        lexer_t::error_message += "\n";
                        return false;
                    }
                }
            } else {
                skip_to_state_evaluation = false;
            }
            
            if (states.empty()) {
                return true;
            }

            if (states.back()) {    // array
                if (get_token() == token_type::data_separator) {
                    get_token();
                    continue;
                }

                if (JSON_LIKELY(last_token == token_type::array_end)) {
                    sax->end_array();

                    if (JSON_UNLIKELY(states.empty())) {
                        lexer_t::error_message += "states is empty\n";
                        return false;
                    }

                    states.pop_back();
                    skip_to_state_evaluation = true;
                    continue;
                }

                lexer_t::error_message += "invalid last_token, is not array end, last_token = ";
                lexer_t::error_message += token_type_name(last_token);
                lexer_t::error_message += "\n";
                return false;
            }

            // next value
            if (get_token() == token_type::data_separator) {
                // parse key
                if (JSON_UNLIKELY(get_token() != token_type::value_string)) {
                    lexer_t::error_message += "invalid character after ',', is not object key\n";
                    return false;
                }

                if (JSON_UNLIKELY(!sax->key(std::move(lexer_t::get_string())))) {
                    lexer_t::error_message += "nodes is empty or key exsit\n";
                    return false;
                }

                // parse separator (:)
                if (JSON_UNLIKELY(get_token() != token_type::object_separator)) {
                    
                    return false;
                }

                get_token();
                continue;
            }

            if (JSON_LIKELY(last_token == token_type::object_end)) {
                sax->end_object();

                if (JSON_UNLIKELY(states.empty())) {
                    lexer_t::error_message += "states is empty\n";
                    return false;
                }
                states.pop_back();
                skip_to_state_evaluation = true;
                continue;
            }

            lexer_t::error_message += "error token type, is not object end, last_token = ";
            lexer_t::error_message += token_type_name(last_token);
            lexer_t::error_message += "\n";
            return false;
        }
    }
private:
    token_type last_token;
    const parser_callback_t<BasicJsonType> callback_ = nullptr;
};
    
} // namespace json
