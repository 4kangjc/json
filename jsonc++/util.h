#pragma once

#include <vector>
#include <string>

namespace json {

//获得函数调用堆栈信息
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
//获得函数调用堆栈信息字符串
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

}