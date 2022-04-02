#include <jsoncc/json>

int main() {
    json::value json;

    auto& array = json["array"];
    array.push_back(280);
    array.emplace_back(true);
    array.push_back("Hello World!");

    if (array.back().is_string()) {
        array.pop_back();
    }

    json::value object;
    object["name"] = "system";
    object["type"] = "file";
    object.emplace("level", "debug");
    if (auto [lit, ok] = object.try_emplace("level", "info"); !ok) {
        std::cout << "try_emplace fail, key exsit" << std::endl;
    }

    if (auto [lit, ok] = object.insert_or_assign("level", "warning"); !ok) {
        std::cout << "insert fail, value change, now value = " << *lit << std::endl;
    } else {
        std::cout << "insert success";
    }

    for (auto& value : array) {                         // for (std::size_t i = 0; i < array.size(); ++i) { 
        std::cout << value << std::endl;                //    std::cout << array[i] << std::endl;
    }                                                   // }

    for (auto it = object.begin(); it != object.end(); ++it) {
        std::cout << it.key() << " - " << *it << std::endl;
    }

    std::cout << array[0].as<int>() << std::endl;
    if (array[1].is<bool>()) {
        std::cout << array[1].as_boolean() << std::endl;
    }

    if (auto it = object.find("level"); it != object.end()) {
        std::cout << "key = level, value = " << *it << std::endl;
        object.erase(it);
    }

    json.push_back({"object", std::move(object)});      // equal map insert

    std::cout << json << std::endl;
}