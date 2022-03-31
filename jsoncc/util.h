#pragma once

#include <vector>
#include <string>
#include <cxxabi.h>
#include <execinfo.h>
#include <sstream>

namespace json {

static std::string damangle(const char* str) {
    size_t size = 0;
    int status = 0;
    std::string rt;
    rt.resize(256);
    if (1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", rt.data())) {
        char* v = abi::__cxa_demangle(rt.data(), nullptr, &size, &status);
        if (v) {
            std::string result(v);
            free(v);
            return result;
        } 
    }
    if (1 == sscanf(str, "%255s", rt.data())) {
        return rt;
    }
    return str;
}

//获得函数调用堆栈信息
static void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1) {
    auto array = (void**)malloc(size * sizeof(void*));
    size_t s = ::backtrace(array, size);
    char** strings = backtrace_symbols(array, s);
    if (strings == nullptr) {
        // TODO LOG << "backtrace_symbols error";
        std::cout << __FILE__ << " : " << __LINE__ << " " << "backtrace_symbols error" << std::endl;
        return;
    }
    for (size_t i = skip; i < s; ++i) {
        // bt.push_back(strings[i]);
        bt.push_back(damangle(strings[i]));
    }
    free(strings);
    free(array);
}
//获得函数调用堆栈信息字符串
static std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "") {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

}