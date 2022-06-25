#pragma once

#include <rich-log/domain.hh>
#include <rich-log/location.hh>

// TODO: maybe provide a second log header that does not include cc::format?
#include <clean-core/format.hh>

/**
 *
 * general-purpose logging framework
 *
 * Quickstart:
 *
 *    // default logging has info verbosity and goes to Default domain
 *    // logging message is created using cc::format
 *    // use LOGP or LOGF if you only want pythonic or printf syntax
 *    LOG("created %s vertices and {} faces", v_cnt, f_cnt);
 *
 *    // for quick debugging, use the following shortcut
 *    int x = 17;
 *    int y = 10;
 *    LOG_EXPR(x + y); // logs "x + y = 27"
 *
 *    // more sophisticated logging uses the long form with explicit domain and severity
 *    LOGD(MyDomain, Warning, "file '%s' is empty", filename);
 *
 *    // custom domains must be DECLARED before they can be used
 *    // this is very lightweight (APIs can declare domains in public headers just fine)
 *    // a domain is declared by:
 *    RICH_LOG_DECLARE_DOMAIN(MyDomain);
 *
 *    // custom domains can be nested:
 *    RICH_LOG_DECLARE_DOMAIN(MyDomain::SystemA::ComponentB);
 *
 *    // per default, Trace is disabled compile-time
 *    // a custom minimum compile-time verbosity level for your domain can be specified via
 *    RICH_LOG_DECLARE_DOMAIN_MV(MyDomain, Warning);
 */

#define RICH_LOG_IMPL(Domain, Severity, Formatter, ...)                                                                                      \
    do                                                                                                                                       \
    {                                                                                                                                        \
        if constexpr (rlog::domains::Domain::CompileTimeMinVerbosity >= rlog::verbosity::Severity)                                           \
        {                                                                                                                                    \
            if (rlog::domains::Domain::domain.verbosity >= rlog::verbosity::Severity)                                                        \
            {                                                                                                                                \
                static constexpr rlog::location _rlog_location = RLOG_LOCATION();                                                            \
                if (rlog::detail::do_log(rlog::domains::Domain::domain, rlog::verbosity::Severity, &_rlog_location, Formatter(__VA_ARGS__))) \
                    CC_DEBUG_BREAK();                                                                                                        \
            }                                                                                                                                \
        }                                                                                                                                    \
    } while (0) // force ;

/// writes an info log message to the Default domain using cc::format (printf AND pythonic syntax)
#define RICH_LOG(...) RICH_LOG_IMPL(Default, Info, cc::format, __VA_ARGS__)
/// writes an info log message to the Default domain using cc::formatp (pythonic  syntax only)
#define RICH_LOGP(...) RICH_LOG_IMPL(Default, Info, cc::formatp, __VA_ARGS__)
/// writes an info log message to the Default domain using cc::formatf (printf syntax only)
#define RICH_LOGF(...) RICH_LOG_IMPL(Default, Info, cc::formatf, __VA_ARGS__)
/// writes a log message with given domain and severity using cc::format
#define RICH_LOGD(Domain, Severity, ...) RICH_LOG_IMPL(Domain, Severity, cc::format, __VA_ARGS__)
/// convenience wrapper for LOG("<expr> = %s", <expr>)
#define RICH_LOG_EXPR(...) RICH_LOG("%s = %s", #__VA_ARGS__, __VA_ARGS__)

// TODO: do we really want to provide _WARN, _ERROR or require full domain use for that?
//      (pt) I would prefer full domain use, so the default domain is not spammed

#ifndef RICH_LOG_FORCE_MACRO_PREFIX

/// writes an info log message to the Default domain using cc::format (printf AND pythonic syntax)
#define LOG(...) RICH_LOG_IMPL(Default, Info, cc::format, __VA_ARGS__)
/// writes an info log message to the Default domain using cc::formatp (pythonic  syntax only)
#define LOGP(...) RICH_LOG_IMPL(Default, Info, cc::formatp, __VA_ARGS__)
/// writes an info log message to the Default domain using cc::formatf (printf syntax only)
#define LOGF(...) RICH_LOG_IMPL(Default, Info, cc::formatf, __VA_ARGS__)
/// writes a log message with given domain and severity using cc::format
#define LOGD(Domain, Severity, ...) RICH_LOG_IMPL(Domain, Severity, cc::format, __VA_ARGS__)
/// convenience wrapper for LOG("<expr> = %s", <expr>)
#define LOG_EXPR(...) RICH_LOG("%s = %s", #__VA_ARGS__, __VA_ARGS__)

#endif

namespace rlog::detail
{
/// NOTE: loc is a pointer to the data segment (static lifetime)
/// TODO: we might be able to improve performance by providing a threadlocal stream_ref<char> to the formatter
/// returns true if we want to hit a breakpoint after logging
bool do_log(rlog::domain_info const& domain, rlog::verbosity::type verbosity, location const* loc, cc::string_view message);
}
