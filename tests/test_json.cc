#include "jsonc++/json.h"
#include <iostream>

int main() {
    json::value value;
    value["Hello"] = "world";
    value["array"].push_back("array_1");
    value["array"].push_back("array_2");
    value["boolean"] = true;
    value["num_int"] = 1;
    value["num_uint"] = 2u;
    value["num_real"] = 5.20;
    value["array"][3] = "array_3";
    value["array"][4] = 4;
    // std::cout << value["Hello"].type_name() << std::endl;
    std::cout << value["Hello"].as_cstring() << std::endl;
    std::cout << value["boolean"].as_boolean() << std::endl;
    std::cout << value["num_int"].as_int() << std::endl;
    std::cout << value["num_uint"].as_uint() << std::endl;
    std::cout << value["num_real"].as_string() << std::endl;
}