#include <chrono>
#include <iostream>
#include <json/json.h>
#include <nlohmann/json.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
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
        for (int i = 1; i <= 1000; ++i) {
            Json::Reader r;
            Json::Value v;
            std::string file = "bin/conf/test" + std::to_string(i % 10) + ".json";
            std::ifstream is(file);
            r.parse(is, v);

            // std::string out_file = "bin/temp/test_jsoncpp" + std::to_string(i) + ".json";
            // std::ofstream os(out_file);
            // Json::FastWriter w;
            // // Json::StyledWriter sw;
            // // os << sw.write(v);
            // os << w.write(v);
        }
    }

    {
        Timer t;
        for (int i = 1; i <= 1000; ++i) {
            std::string file = "bin/conf/test" + std::to_string(i % 10) + ".json";
            std::ifstream is(file);
            auto json = nlohmann::json::parse(is);

            // std::string out_file = "bin/temp/test_nlohmannjson" + std::to_string(i) + ".json";
            // std::ofstream os(out_file);
            // os << json.dump();
        }
    }

    {
        Timer t;
        for (int i = 1; i <= 1000; ++i) {
            std::string file = "bin/conf/test" + std::to_string(i % 10) + ".json";
            // rapidjson::StringStream is(file.data());
            std::ifstream is(file);
            std::string s(std::istreambuf_iterator<char>(is), {});
            rapidjson::Document d;
            // d.ParseStream(is);
            d.Parse(s.c_str());

            // std::string out_file = "bin/temp/test_rapidjson" + std::to_string(i) + ".json";
            // std::ofstream os(out_file);

            // rapidjson::StringBuffer buffer;
            // rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            // d.Accept(writer);

            // os << buffer.GetString() << std::endl;
        } 
    }

    {
        // std::string file = "bin/conf/log.json";
        // std::ifstream is(file);
        // auto json = json::value::parse(is);
        Timer t;
        for (int i = 1; i <= 1000; ++i) {
            std::string file = "bin/conf/test" + std::to_string(i % 10) + ".json";
            std::ifstream is(file);
            auto json = json::value::parse(is);

            // std::string out_file = "bin/temp/test_jsoncc" + std::to_string(i) + ".json";
            // std::ofstream os(out_file);
            // json->dump(os);
            // json->write<true>(os);
        }
    }
}