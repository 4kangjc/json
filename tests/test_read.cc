#include <iostream>
#include <fstream>
#include <string>
#include "jsonc++/json.h"

void print_json(json::value& json, int level) {
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
                std::cout << std::string(4 * level, ' ') << it.key() << " - " << it->type_name() << " - " << level << std::endl;
                print_json(*it, level + 1);
            }
            break;
        }
        default: {
            std::cout << std::string(4 * level, ' ') << json.as_string() << " - " << json.type_name() << " - " << level << std::endl; 
        }
    }
}

int main(int argc, char** argv) {
    std::ifstream is1("bin/conf/test1.json");
    std::ifstream is2("bin/conf/test2.json");
    std::ifstream is3("bin/conf/test3.json");
    
    std::string s2(std::istreambuf_iterator<char>(is2), {});
    // std::cout << s2 << std::endl;
    // // while (is1 >> s) {
    // //     std::cout << s;
    // // }
    // std::cout << s << std::endl;
    // auto adapter2 = json::input_adapter(s);
    // std::string ss;
    // for (auto c = adapter2.get_next_char(); c != -1; c = adapter2.get_next_char()) {
    //     ss.push_back(c);
    // }

    std::FILE* file = fopen("bin/conf/test4.json", "r");

    auto json = json::value::parse(is1);
    if (json) {
        print_json(*json, 0);
    }
    auto json2 = json::value::parse(s2);
    if (json2) {
        print_json(*json2, 0);
    }
    if (auto json3 = json::value::parse(is3); json3) {
        print_json(*json3, 0);
    }
    if (auto json4 = json::value::parse(file); json4) {
        print_json(*json4, 0);
    }
}