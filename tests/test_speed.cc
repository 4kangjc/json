#include <chrono>
#include <iostream>
#include <json/json.h>
#include <jsoncc/json>
#include <fstream>

struct Timer {
    std::chrono::time_point<std::chrono::system_clock> begin;

    Timer() {
        begin = std::chrono::system_clock::now();
    }
    ~Timer() {
        std::cout << "Timer took " << (std::chrono::system_clock::now() - begin).count() * 1000.0f << "ms" << std::endl;
    }
};

int main() {
    {
        // Json::Reader r;
        // Json::Value v;
        // std::string file = "bin/conf/log.json";
        // std::ifstream is(file);
        // r.parse(is, v);
        Timer t;
        for (int i = 1; i <= 100; ++i) {
            Json::Reader r;
            Json::Value v;
            std::string file = "bin/conf/test" + std::to_string(i % 6 + 1) + ".json";
            std::ifstream is(file);
            r.parse(is, v);

            std::string out_file = "bin/temp/test_jsoncpp" + std::to_string(i) + ".json";
            std::ofstream os(out_file);
            Json::FastWriter w;
            os << w.write(v);
        }
    }

    {
        // std::string file = "bin/conf/log.json";
        // std::ifstream is(file);
        // auto json = json::value::parse(is);
        Timer t;
        for (int i = 1; i <= 100; ++i) {
            std::string file = "bin/conf/test" + std::to_string(i % 6 + 1) + ".json";
            std::ifstream is(file);
            auto json = json::value::parse(is);

            std::string out_file = "bin/temp/test_jsonc++" + std::to_string(i) + ".json";
            std::ofstream os(out_file);
            json->dump(os);
        }
    }
}