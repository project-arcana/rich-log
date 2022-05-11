#include "StdOutLogger.hh"

#ifdef CC_OS_WINDOWS
#include <clean-core/native/win32_sanitized.hh>

// this macro is only defined with SDK versions beyond 10586
// (the call can of course still succeed depending on the version of the executing system)
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#endif

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <clean-core/macros.hh>
#include <clean-core/utility.hh>

#include <rich-log/options.hh>

#define RLOG_COLOR_TIMESTAMP "\u001b[38;5;37m"
#define RLOG_COLOR_RESET "\u001b[0m"

namespace
{
thread_local char tlsThreadName[32] = "";

CC_FORCE_INLINE void write_timebuffer(char* timebuffer, size_t size, char const* format)
{
    std::time_t const t = std::time(nullptr);
#ifdef CC_OS_WINDOWS
    std::tm lt_stack;
    ::localtime_s(&lt_stack, &t);
    std::tm* const lt = &lt_stack;
#else
    std::tm* const lt = std::localtime(&t);
#endif

    std::strftime(timebuffer, size, format, lt);
}
}

rlog::StdOutLogger::StdOutLogger(console_log_style style) : LoggerBase("StdOutLogger"), mLogStyle(style) {}

void rlog::StdOutLogger::log(message_params const& params, char const* msg) { logMessageToStdOut(mLogStyle, params, msg); }

void rlog::logMessageToStdOut(console_log_style style, message_params const& params, char const* msg)
{
    std::FILE* stream = stdout;
    if (params.severity >= severity::warning)
    {
        stream = stderr;
    }

    char timebuffer[64] = "";

    char const* const colorSeverity = getColorANSICode(getSeverityStyle(params.severity));
    char const* const colorDomain = getColorANSICode(params.domain.color);

    char const* const nameSeverity = getSeverityName(params.severity);

    switch (style)
    {
    case console_log_style::verbose_with_location:
        std::fprintf(stream, "         %s:%d  (%s):\n%s\n", params.location.file, params.location.line, params.location.function, msg);
        [[fallthrough]];
    case console_log_style::verbose:
    {
        // prepare timestamp
        write_timebuffer(timebuffer, sizeof(timebuffer), "%d.%m.%y %H:%M:%S");

        // full log line
        // [timestamp]     [thread id] [severity] [domain] [message]
        // 06.05.20 07:14:10 t000      INFO       NET      <the message being printed>\n

        auto domain_name = params.domain.name ? params.domain.name : "LOG";
        std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s %s " RLOG_COLOR_RESET "%s%-7s " RLOG_COLOR_RESET " %s%-9s " RLOG_COLOR_RESET "%s\n", timebuffer,
                     tlsThreadName, colorSeverity, nameSeverity, colorDomain, domain_name, msg);
    }
    break;
    case console_log_style::verbose_no_color:
    {
        // prepare timestamp
        write_timebuffer(timebuffer, sizeof(timebuffer), "%d.%m.%y %H:%M:%S");

        // full log line
        // [timestamp]     [thread id] [severity] [domain] [message]
        // 06.05.20 07:14:10 t000      INFO       NET      <the message being printed>\n

        auto domain_name = params.domain.name ? params.domain.name : "LOG";
        std::fprintf(stream, "%s %s %-7s  %-9s %s\n", timebuffer, tlsThreadName, nameSeverity, domain_name, msg);
    }
    break;
    case console_log_style::brief:
    {
        // prepare timestamp
        write_timebuffer(timebuffer, sizeof(timebuffer), "%H:%M:%S");

        // brief log line
        // [timestamp] [severity] [domain] [message]
        // 07:14:10 INFO [NET] <the message being printed>\n

        // timestamp and severity (always)
        auto length = std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s " RLOG_COLOR_RESET "%s%s " RLOG_COLOR_RESET, timebuffer, colorSeverity, nameSeverity);

        if (params.domain.name != nullptr) // domain, optional
            length += std::fprintf(stream, "%s%s " RLOG_COLOR_RESET, colorDomain, params.domain.name);

        std::fprintf(stream, "%s\n", msg);
    }
    break;
    case console_log_style::briefer:
    {
        // prepare timestamp
        write_timebuffer(timebuffer, sizeof(timebuffer), "%H:%M");

        // briefer log line
        // [timestamp] [severity] [domain] [message]
        // 07:14 I [NET] <the message being printed>\n

        // timestamp and severity (always)
        auto length = std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s " RLOG_COLOR_RESET "%s%c " RLOG_COLOR_RESET, timebuffer, colorSeverity,
                                   nameSeverity[0] ? nameSeverity[0] : ' ');

        if (params.domain.name != nullptr) // domain, optional
            length += std::fprintf(stream, "%s%s " RLOG_COLOR_RESET, colorDomain, params.domain.name);

        std::fprintf(stream, "%s\n", msg);
    }
    break;
    case console_log_style::message_only:
        std::fprintf(stream, "%s\n", msg);
        break;
    default:
        CC_UNREACHABLE("unsupported log style");
    }

    std::fflush(stream);
}

void rlog::setCurrentThreadName(const char* fmt, ...)
{
    if (fmt != nullptr)
    {
        std::va_list args;
        va_start(args, fmt);
        std::vsnprintf(tlsThreadName, sizeof(tlsThreadName), fmt, args);
        va_end(args);
    }
    else
    {
        tlsThreadName[0] = '\0';
    }
}

bool rlog::enableWin32ConsoleColors()
{
#ifdef CC_OS_WINDOWS
    ::HANDLE const console_handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (console_handle == INVALID_HANDLE_VALUE)
        return false;

    ::DWORD prev_mode;
    if (!::GetConsoleMode(console_handle, &prev_mode))
        return false;

    prev_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!::SetConsoleMode(console_handle, prev_mode))
        return false;

    return true;
#else
    return true;
#endif
}
