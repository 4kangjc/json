#include "jsoncc/meta.h"
#include <iostream>

int main() {
    std::cout << std::boolalpha;
    std::cout << json::meta::is_string_v<const char*> << std::endl;                 // false
    std::cout << json::meta::is_string_v<std::string> << std::endl;                 // true
    std::cout << json::meta::is_string_v<std::vector<char>> << std::endl;           // true
    std::cout << json::meta::is_string_v<std::string_view> << std::endl;            // false
    std::cout << json::meta::is_string_v<std::wstring> << std::endl;                // true
    std::cout << json::meta::is_string_v<char> << std::endl;                        // false
    std::cout << json::meta::is_cstring_v<char*> << std::endl;                      // true
    std::cout << json::meta::is_cstring_v<char[3]> << std::endl;                    // true
    std::cout << json::meta::is_cstring_v<const char[]> << std::endl;               // true
    std::cout << json::meta::is_cstring_v<const char[423423]> << std::endl;         // true
    std::cout << json::meta::is_string_v<const std::string> << std::endl;           // true
    std::cout << json::meta::is_string_v<std::string&> << std::endl;                // true
    std::cout << json::meta::is_string_v<std::string&&> << std::endl;               // true
    std::cout << json::meta::is_cstring_v<volatile wchar_t*> << std::endl;          // true
    std::cout << json::meta::is_string_v<std::vector<char16_t>> << std::endl;       // true
    std::cout << json::meta::is_string_v<std::array<char32_t, 32>> << std::endl;    // false
    std::cout << json::meta::is_cstring_v<std::array<char32_t, 32>> << std::endl;   // true
    std::cout << json::meta::is_cstring_v<const char* const> << std::endl;          // true
    std::cout << json::meta::is_cstring_v<const char(&)[]> << std::endl;            // true
}