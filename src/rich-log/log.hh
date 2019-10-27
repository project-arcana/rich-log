#pragma once

#include <clean-core/macros.hh>

#include <rich-log/MessageBuilder.hh>
#include <rich-log/location.hh>

#ifndef RICH_LOG_FORCE_MACRO_PREFIX

#define LOG(...) RICH_LOG(__VA_ARGS__)
#define LOG_EXPR(expr, ...) RICH_LOG_EXPR(expr, __VA_ARGS__)

#endif

#define RICH_LOG(...)                                                                       \
    [] {                                                                                    \
        using namespace rlog;                                                               \
        static constexpr ::rlog::location _rlog_loc = {CC_PRETTY_FUNC, __FILE__, __LINE__}; \
        return ::rlog::MessageBuilder(_rlog_loc);                                           \
    }()

#define RICH_LOG_EXPR(expr, ...) RICH_LOG(no_sep, ##__VA_ARGS__) << #expr " = " << (expr)
