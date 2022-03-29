#include <jsoncc/json>
#include <deque>
#include <list>
#include <unordered_map>
#include <fstream>

namespace json {

using my_value = json::basic_value<std::map, std::deque, std::vector<char>, bool, int, uint, float, std::allocator>;

}

template <typename String>
std::string like_string_as(String string) {
    std::string s;
    for (char c : string) {
        s.push_back(c);
    }
    return s;
}

template <typename BasicJsonType>
void print_json(BasicJsonType& json, int level) {
    switch (json.type()) {
        case json::value_t::array: {
            for (std::size_t i = 0; i < json.size(); ++i) {
                std::cout << std::string(4 * level, ' ') << i << " - " << json[i].type_name() << " - " << level << std::endl;
                print_json(json[i], level + 1);
            }
            break;
        }
        case json::value_t::object: {
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::cout << std::string(4 * level, ' ') << like_string_as(it.key()) << " - " << it->type_name() << " - " << level << std::endl;
                print_json(*it, level + 1);
            }
            break;
        }
        case json::value_t::number_int: {
            std::cout << std::string(4 * level, ' ') << json.as_int() << " - " << json.type_name() << " - " << level << std::endl;
            break;
        }
        case json::value_t::number_uint: {
            std::cout << std::string(4 * level, ' ') << json.as_uint() << " - " << json.type_name() << " - " << level << std::endl;
            break;
        }
        case json::value_t::number_real: {
            std::cout << std::string(4 * level, ' ') << json.as_real() << " - " << json.type_name() << " - " << level << std::endl;
            break;
        }
        case json::value_t::string: {
            std::cout << std::string(4 * level, ' ') << json.as_cstring() << " - " << json.type_name() << " - " << level << std::endl;
            break;
        }
        case json::value_t::boolean: {
            std::cout << std::string(4 * level, ' ') << std::boolalpha << json.as_boolean() << " - " << json.type_name() << " - " << level << std::endl;
            break;
        }
        case json::value_t::null: {
            std::cout << std::string(4 * level, ' ') << "null" << " - " << json.type_name() << " - " << level << std::endl;
            break;
        }
        default: {
            std::cout << "invalid type, type = " << json.type_name() << std::endl;
        }
    }
}

int main() {
    json::value root;
    root["array"].push_back(1);
    std::cout << root["array"].size() << std::endl;
    std::cout << root["array"].back().as<int>() << std::endl;
    root["array"].emplace_back("hello world!"); 
    std::cout << root["array"].back().as<const char*>() << std::endl;
    std::cout << root["array"].front().as<unsigned long long>() << std::endl;
    root["array"].push_back(true);
    auto& array = root["array"];
    bool b = array.back().as<bool>();
    std::cout << b << std::endl;
    // auto cstr = array[1].as<char*>();
    // std::cout << cstr << std::endl;
    // cstr = "world hello!";
    // cstr[4] = 'a';
    std::cout << array[1].as_string() << std::endl;
    std::cout << array.is<json::value::array_t>() << std::endl;
    std::cout << array[1].is<std::string_view>() << std::endl;
    std::cout << array[0].is<char>() << std::endl;
    array.push_back(5.20);
    std::cout << array.back().is<long double>() << std::endl;
    // root["array"] = nullptr;
    std::cout << root["array"].back().as<std::string>() << std::endl;
    std::cout << std::is_same_v<bool, char> << std::endl;
    

    std::ifstream is("bin/conf/test2.json");
    if (auto json = json::my_value::parse(is); json) {
        print_json(*json, 0);
    }

}