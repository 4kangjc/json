# jsoncc

## 介绍
jsoncc是一个轻量的，快速的解析并生成json的c++库，它具有c++ stl的api风格，轻松上手，header only, 没有库，没有依赖项

## 编译
* 编译环境
`gcc version 11.2.0   (最低 gcc 7.1)`  

* 编译运行示例
```shell
git clone https://github.com/4kangjc/json.git
cd json
make
bin/json | bin/read
```

## 代码示例
```cpp
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
        std::cout << "try_emplace fail, key exists" << std::endl;
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
```

```cpp
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
```

## 速度测试
```
bin/test_speed
Timer took 6.58569e+10ms            <-- jsoncpp
Timer took 5.02203e+10ms            <-- nlohmann json
Timer took 1.82665e+10ms            <-- rapid json
Timer took 3.73444e+10ms            <-- jsoncc [this project]
```

## 参考
* [nlohmann json](https://github.com/nlohmann/json)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)