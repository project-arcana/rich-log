#pragma once

#include <cstdio>

#include <clean-core/macros.hh>

#include <rich-log/options.hh>

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
};

/// prints a formatted, colored prefix to the specified stream, of the form
/// (or slightly different depending on log style)
/// returns length of prefix
int print_prefix_to_stream(severity severity, domain domain, std::FILE* stream = stdout);

/// sets the name for the calling thread, as it appears in the log prefix, using printf syntax
/// pass nullptr to un-set
void set_current_thread_name(char const* fmt, ...) CC_PRINTF_FUNC(1, 2);

/// changes the way print_to_console formats its output
void set_console_log_style(console_log_style style);

/// enables ANSI Escape sequences in Windows conhost.exe and cmd.exe
bool enable_win32_colors();
}
