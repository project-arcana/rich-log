#pragma once

#include <rich-log/LoggerBase.hh>

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

class RLOG_API StdOutLogger final : public LoggerBase
{
public:
    explicit StdOutLogger(console_log_style style);

    void log(message_params const& params, char const* msg) override;

    void setLogStyle(console_log_style style) { mLogStyle = style; }

private:
    console_log_style mLogStyle = console_log_style::verbose;
};

RLOG_API void logMessageToStdOut(console_log_style style, message_params const& params, char const* msg);

/// sets the name for the calling thread, as it appears in the stdout logger prefix, using printf syntax
/// pass nullptr to un-set
RLOG_API void setCurrentThreadName(const char* fmt, ...) CC_PRINTF_FUNC(1);

/// enables ANSI Escape sequences in Windows conhost.exe and cmd.exe
RLOG_API bool enableWin32ConsoleColors();
}