#include "util.h"
#include <cxxabi.h>
#include <sstream>
#include <execinfo.h>

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

void Backtrace(std::vector<std::string>& bt, int size, int skip) {
    auto array = (void**)malloc(size * sizeof(void*));
    size_t s = ::backtrace(array, size);
    char** strings = backtrace_symbols(array, s);
    if (strings == NULL) {
        // FLEXY_LOG_ERROR(g_logger) << "backtrace_symbols eroor";
        return;
    }
    for (size_t i = skip; i < s; ++i) {
        // bt.push_back(strings[i]);
        bt.push_back(damangle(strings[i]));
    }
    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

}