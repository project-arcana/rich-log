#pragma once

#include <cstdio>

#include <clean-core/macros.hh>
#include <clean-core/unique_function.hh>

#include <rich-log/detail/api.hh>
#include <rich-log/domain.hh>
#include <rich-log/location.hh>
#include <rich-log/message.hh>

namespace rlog
{
enum class console_log_style
{
    // [timestamp]     [thread id] [severity] [domain] [message]
    // 06.05.20 07:14:10 t000      INFO       NET      <the message being printed>\n
    verbose,

    // [timestamp] [severity] [domain (optional)] [message]
    // 07:14:10 INFO NET <the message being printed>\n
    brief,

    // [timestamp] [severity] [domain (optional)] [message]
    // 07:14 I NET <the message being printed>\n
    briefer,

    // [message]
    // <the message being printed>\n
    message_only,

    // like verbose, but without color. useful if running inside a terminal
    // that performs poorly with color codes, like qt creators builtin terminal
    verbose_no_color,

    // like verbose, but also including filename:line for the log location
    verbose_with_location
};

/// sets the name for the calling thread, as it appears for the default logger in the log prefix, using printf syntax
/// pass nullptr to un-set
RLOG_API void set_current_thread_name(char const* fmt, ...) CC_PRINTF_FUNC(1);

/// changes the way print_to_console formats its output
[[deprecated("replace the default logger instead. 2022-06-25")]] //
RLOG_API void
set_console_log_style(console_log_style style);

/// enables ANSI Escape sequences in Windows conhost.exe and cmd.exe
RLOG_API bool enable_win32_colors();

/// sets a global minimum verbosity that will trigger breakpoints
/// e.g. rlog::set_break_on_log_minimum_verbosity(rlog::verbosity::Warning);
///      will break on every warning, error, or fatal LOG
/// default is Fatal
RLOG_API void set_break_on_log_minimum_verbosity(verbosity::type v);

/// sets the global default logger (i.e. the fallback when no local logger overwrite is provided)
/// CAUTION: this function must be externally synchronized
///          no LOG call must happen (e.g. in a different thread) at the same time
///          usually, this is called once at the start of main and otherwise not touched
RLOG_API void set_global_default_logger(cc::unique_function<void(message_ref msg)> logger);

/// pushes a logger onto the threadlocal log overwrite stack
/// i.e. all subsequent LOG calls on the current thread are routed to this logger (unless further overwritten)
/// NOTE: rlog::overwrite_logger can be used for automatic scoping
RLOG_API void push_local_logger(cc::unique_function<void(message_ref msg)> logger);

/// pops a logger from the threadlocal log overwrite stack
RLOG_API void pop_local_logger();

/// helper struct for a threadlocal scoped log overwrite
/// Usage:
///
///   auto _ = rlog::overwrite_logger([](rlog::message_ref msg) {
///       custom_log(msg.message);
///   });
///
struct RLOG_API overwrite_logger
{
    [[nodiscard]] explicit overwrite_logger(cc::unique_function<void(message_ref msg)> logger) { push_local_logger(cc::move(logger)); }

    ~overwrite_logger() { pop_local_logger(); }

    overwrite_logger(overwrite_logger&&) = delete;
    overwrite_logger& operator=(overwrite_logger&&) = delete;
    overwrite_logger(overwrite_logger const&) = delete;
    overwrite_logger& operator=(overwrite_logger const&) = delete;
};
}
