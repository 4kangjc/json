#include <jsoncc/json>
#include <fstream>

using my_json = json::basic_value<std::map, std::vector, std::vector<char>>;

int main() {
    std::ifstream is("bin/conf/log.json");
    auto json_ptr = my_json::parse(is);
    std::string s;
    if (json_ptr) {
        json_ptr->write(s);
        std::cout << s << std::endl;
        // can not use `std::cout << *json_ptr`   because `std::cout << std::vector<char>`
    }
}