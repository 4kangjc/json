#include <jsoncc/json>
#include <fstream>

namespace json {
    using my_value = json::basic_value<std::map, std::vector, std::wstring>;
};

int main() {
    std::ios_base::sync_with_stdio(false); 
    std::wcout.imbue(std::locale("zh_CN.UTF-8"));
    // std::vector<char> v;
    // json::output_adapter<char, std::vector<char>> ad(v);
    // // nlohmann::detail::output_adapter<char, std::vector<char>> ad(v);
    std::basic_ifstream<char> is("bin/conf/test8.json");
    auto json = json::my_value::parse(is);
    // // // std::vector<char> s;
    // json->dump(std::wcout);
    // std::wcout << std::endl;
    // std::wstring s;
    // json::my_value ws_j = L"你好， 世界！";
    std::wofstream wos("bin/conf/test9.json");
    wos.imbue(std::locale("zh_CN.UTF-8"));
    // json::writer<json::my_value, false> writer(wos);
    // auto& real_json = *json;
    // real_json[L"sites"][0][L"name"] = L"菜鸟教程";
    // real_json[L"sites"][2][L"name"] = L"微博";
    // writer.dump(*json);
    // writer.dump(ws_j);
    // writer.dump(real_json);
    json->write<true>(wos);
    // std::wcout << s << std::endl;

    // sizeof(std::byte)
    // std::string reals;
    // std::wstring ws = L"this is 菜鸟教程";
    // std::string ss = "this is 你好";
    // std::cout << ss << std::endl;
    // std::wcout << ws << std::endl;
    // std::cout << sizeof("null") << std::endl;
    // for (char c : s) {
        // reals.push_back(c);
        // std::cout << c;
    // }
    
    
    // std::vector<char> v = "fds";
    // std::cout << std::boolalpha << (ss.str() == s) << std::endl;
    return 0;
}

int main2() {
    for (int i = 1; i <= 6; ++i) {
        std::string file = "bin/conf/test" + std::to_string(i) + ".json";
        std::ifstream is(file);
        auto json_ptr = json::value::parse(is);

        std::string out_file = "bin/conf/temp" + std::to_string(i) + ".json";  
        std::ofstream os(out_file);
        // json::writer<json::value, true> writer(os);
        // writer.dump(*json_ptr);
        json_ptr->write<true>(json::output_adapter<char>(os));
        // json_ptr->write<true>(os);
    }
    return 0;
}