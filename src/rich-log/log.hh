#pragma once

#include <rich-log/detail/log_impl.hh>

namespace rlog
{
inline void debug_functor(MessageBuilder& builder)
{
    builder.set_severity(severity::debug);
    builder.set_must_be_whitelisted(true);
}

inline void trace_functor(MessageBuilder& builder) { builder.set_severity(severity::trace); }

inline void warning_functor(MessageBuilder& builder) { builder.set_severity(severity::warning); }

inline void error_functor(MessageBuilder& builder) { builder.set_severity(severity::error); }

inline void expression_functor(MessageBuilder& builder) { builder.set_separator(""); }
}

#ifndef RICH_LOG_FORCE_MACRO_PREFIX

#define LOG RICH_LOG_IMPL(nullptr)

#define LOG_INFO RICH_LOG_IMPL(nullptr)
#define LOG_WARN RICH_LOG_IMPL(rlog::warning_functor)
#define LOG_TRACE RICH_LOG_IMPL(rlog::trace_functor)
#define LOG_ERROR RICH_LOG_IMPL(rlog::error_functor)
#define LOG_DEBUG RICH_LOG_IMPL(rlog::debug_functor)
#define LOG_DEBUG_DOMAIN(DomainName) RICH_LOG_DEBUG_DOMAIN(DomainName)

#define LOG_EXPR(expr, ...) RICH_LOG_EXPR(expr, ##__VA_ARGS__)

#endif

#define RICH_LOG RICH_LOG_IMPL(nullptr)

#define RICH_LOG_INFO RICH_LOG_IMPL(nullptr)
#define RICH_LOG_WARN RICH_LOG_IMPL(rlog::warning_functor)
#define RICH_LOG_TRACE RICH_LOG_IMPL(rlog::trace_functor)
#define RICH_LOG_ERROR RICH_LOG_IMPL(rlog::error_functor)
#define RICH_LOG_DEBUG RICH_LOG_IMPL(rlog::debug_functor)
#define RICH_LOG_DEBUG_DOMAIN(DomainName)                   \
    RICH_LOG_IMPL(                                          \
        [](::rlog::MessageBuilder& builder)                 \
        {                                                   \
            ::rlog::debug_functor(builder);                 \
            builder.set_domain(::rlog::domain(DomainName)); \
        })

#define RICH_LOG_EXPR(expr, ...) RICH_LOG_IMPL(rlog::expression_functor)(##__VA_ARGS__) << #expr " = " << (expr)
