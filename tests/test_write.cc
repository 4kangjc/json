#include <jsoncc/json>
#include <fstream>
#include <boost/lexical_cast.hpp>
// #include <nlohmann/json.hpp>

namespace json {
    using my_value = json::basic_value<std::map, std::vector, std::string>;
};

int main() {
    // std::vector<char> v;
    // json::output_adapter<char, std::vector<char>> ad(v);
    // nlohmann::detail::output_adapter<char, std::vector<char>> ad(v);
    std::ifstream is("bin/conf/test1.json");
    auto json = json::my_value::parse(is);
    // std::vector<char> s;
    std::string s;
    json::writer<json::my_value, false> writer(s);
    writer.dump(*json);
    std::cout << s << std::endl;
    // sizeof(std::byte)
    // std::string reals;
    // std::string ws = "菜鸟教程";
    // std::cout << ws << std::endl;
    // std::cout << sizeof("null") << std::endl;
    // for (char c : s) {
        // reals.push_back(c);
        // std::cout << c;
    // }
    // std::cout << reals << std::endl;
    // const unsigned char* hello = reinterpret_cast<const unsigned char*>("123");
    
    // std::stringstream ss;
    // ss << hello;
    // int a;
    // ss >> a;
    // std::cout << a << std::endl;
    //std::cout << hello << std::endl;
    //char c = 129;
    //std::cout << c << std::endl;
    // std::stringstream ss;
    // json->dump(std::cout);
    // std::cout << std::endl;
    
    // std::vector<char> v = "fds";
    // std::cout << std::boolalpha << (ss.str() == s) << std::endl;
    // std::wstring ws = L"456";
    // // std::cout << ws << std::endl;
    // // std::wstringstream sa;
    // std::basic_stringstream<wchar_t> sa;
    // sa << ws;
    // int b;
    // sa >> b;
    // // int b = boost::lexical_cast<int>(ws);
    // std::cout << b << std::endl;
    // ss << ws;
    // int b;
    // ss >> b;
}