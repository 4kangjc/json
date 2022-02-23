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

    for (auto& val : value["array"]) {
        std::cout << val.as_string() << std::endl;
    }

    value["map"]["1"] = 1;
    value["map"]["two"] = 2.0f;
    value["map"]["three"] = 3u;
    value["map"]["json"] = "json";
    value["map"]["boolean"] = true;

    auto& map = value["map"];
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        std::cout << it.key() << " - " << it->as_string() << std::endl;
    }

    // for (auto it = map.rbegin(); it != map.rend(); ++it) {
    //     std::cout << it.key() << " - " << it->as_string() << std::endl;
    // }
    if (auto it = map.find("boolean"); it != map.cend()) {
        std::cout << it.key() << " - " << it->as_boolean() << std::endl;
    } else {
        std::cout << "key = boolean is not exit" << std::endl;
    }

    if (!map.count("son")) {
        std::cout << "key = son is not exit" << std::endl;
    }

    auto [it, ok] = map.try_emplace("three", 4u);
    if (ok) {
        std::cout << it.key() << "- " << it->as_string() << std::endl;
    } else {
        std::cout << "try emplace fail, key = " << it.key() << " exit, value = " << it->as_string() << std::endl;
    }

    std::cout << map["three"].as<int>() << std::endl;

}