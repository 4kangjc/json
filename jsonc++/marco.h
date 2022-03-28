#pragma once

#include <iostream>
#include "util.h"

#if defined  __GNUC__ || defined __llvm__ 
// 告诉编译器优化,条件大概率成立
#   define JSON_LIKELY(x)       __builtin_expect(!!(x), 1)
// 告诉编译器优化,条件大概率不成立
#   define JSON_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#   define JSON_LIKELY(x)          (x)
#   define JSON_UNLIKELY(x)        (x)
#endif

#define JSON_ASSERT(x) \
    if (JSON_UNLIKELY(!(x))) { \
        std::cout << __FILE__ << " : " << __LINE__ << ", ASSERTION: " << #x \
            << "\nbacktrace:\n" << json::BacktraceToString(100, 2, "    ");  \
        assert(x); \
    }

#define JSON_ASSERT2(x, w) \
    if (JSON_UNLIKELY((!(x)))) { \
        std::cout << __FILE__ << " : " << __LINE__ << ", ASSERTION: " << #x << '\n' << w   \
            << "\nbacktrace:\n" << json::BacktraceToString(100, 2, "    ");  \
        assert(x); \
    }

#define JSON_LOG(msg) \
    std::cout << __FILE__ << " : " << __LINE__ << " " << msg;
    
#ifdef JSON_USE_EXCEPTION

#define JSON_ERROR(condition) 
#define JSON_ERROR_MSG(condition, message)

#else

#define JSON_ERROR(condition)   JSON_ASSERT(condition)
#define JSON_ERROR_MSG(condition, message) JSON_ASSERT2(condition, message)

#endif 