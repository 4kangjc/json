#include <jsonc++/json>
#include <deque>
#include <list>
#include <unordered_map>

namespace json {

using my_value = json::basic_value<std::map, std::deque, std::string, bool, int, uint, float, std::allocator>;

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
    auto cstr = array[1].as<char*>();
    std::cout << cstr << std::endl;
    // cstr = "world hello!";
    cstr[4] = 'a';
    std::cout << array[1].as_string() << std::endl;
    std::cout << array.is<json::value::array_t>() << std::endl;
    std::cout << array[1].is<std::string_view>() << std::endl;
    std::cout << array[0].is<char>() << std::endl;
    array.push_back(5.20);
    std::cout << array.back().is<long double>() << std::endl;
    // root["array"] = nullptr;
    std::cout << root["array"].back().as<std::string>() << std::endl;
    std::cout << std::is_same_v<bool, char> << std::endl;
    

    // std::cout << json::meta::is_string_v<const char*> << std::endl;
    // std::cout << json::meta::is_string_v<std::string> << std::endl;
    // std::cout << json::meta::is_string_v<std::vector<char>> << std::endl;
    // std::cout << json::meta::is_string_v<std::string_view> << std::endl;
    // std::cout << json::meta::is_string_v<std::wstring> << std::endl;
    // std::cout << json::meta::is_string_v<char> << std::endl;
    // std::cout << json::meta::is_cstring_v<char*> << std::endl;
    // std::cout << json::meta::is_cstring_v<char[3]> << std::endl;
    // std::cout << json::meta::is_cstring_v<const char[]> << std::endl;
    // std::cout << json::meta::is_cstring_v<const char[423423]> << std::endl;
}