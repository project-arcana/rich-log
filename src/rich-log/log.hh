#pragma once

#include <rich-log/detail/log_impl.hh>

namespace rlog
{
constexpr void debug_functor(MessageBuilder& builder)
{
    //
    builder.set_severity(severity::debug());
}

constexpr void warning_functor(MessageBuilder& builder)
{
    builder.set_severity(severity::warning());
    builder.set_use_error_stream(true);
}

constexpr void error_functor(MessageBuilder& builder)
{
    builder.set_severity(severity::error());
    builder.set_use_error_stream(true);
}

constexpr void expression_functor(MessageBuilder& builder)
{
    //
    builder.set_separator("");
}
}

#ifndef RICH_LOG_FORCE_MACRO_PREFIX

#define LOG RICH_LOG_IMPL(nullptr)

#define LOG_INFO RICH_LOG_IMPL(nullptr)
#define LOG_DEBUG RICH_LOG_IMPL(rlog::debug_functor)
#define LOG_WARN RICH_LOG_IMPL(rlog::warning_functor)
#define LOG_ERROR RICH_LOG_IMPL(rlog::error_functor)

#define LOG_EXPR(expr, ...) RICH_LOG_EXPR(expr, ##__VA_ARGS__)

#endif

#define RICH_LOG RICH_LOG_IMPL(nullptr)

#define RICH_LOG_INFO RICH_LOG_IMPL(nullptr)
#define RICH_LOG_DEBUG RICH_LOG_IMPL(rlog::debug_functor)
#define RICH_LOG_WARN RICH_LOG_IMPL(rlog::warning_functor)
#define RICH_LOG_ERROR RICH_LOG_IMPL(rlog::error_functor)

#define RICH_LOG_EXPR(expr, ...) RICH_LOG_IMPL(rlog::expression_functor)(##__VA_ARGS__) << #expr " = " << (expr)
