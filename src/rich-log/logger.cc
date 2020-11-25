#include "logger.hh"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <clean-core/macros.hh>
#include <clean-core/utility.hh>

#ifdef CC_OS_WINDOWS
#include <clean-core/native/win32_sanitized.hh>

// this macro is only defined with SDK versions beyond 10586
// (the call can of course still succeed depending on the version of the executing system)
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#endif

#define RLOG_COLOR_TIMESTAMP "\u001b[38;5;37m"
#define RLOG_COLOR_RESET "\u001b[0m"

namespace
{
thread_local char tls_thread_name[32] = "";
rlog::console_log_style g_log_style = rlog::console_log_style::verbose;

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
    timebuffer[std::strftime(timebuffer, size, format, lt)] = '\0';
}
}

int rlog::print_prefix_to_stream(const location& location, rlog::severity severity, rlog::domain domain, std::FILE* stream)
{
    (void)location; // unused

    switch (g_log_style)
    {
    case console_log_style::verbose_with_location:
        std::fprintf(stream, "         %s:%d  (%s):\n", location.file, location.line, location.function);
        // fall through
    case console_log_style::verbose:
    {
        // prepare timestamp
        char timebuffer[18];
        write_timebuffer(timebuffer, sizeof(timebuffer), "%d.%m.%y %H:%M:%S");

        // full log line
        // [timestamp]     [thread id] [severity] [domain] [message]
        // 06.05.20 07:14:10 t000      INFO       NET      <the message being printed>\n

        auto domain_name = domain.value ? domain.value : "LOG";
        return std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s %s " RLOG_COLOR_RESET "%s%-7s " RLOG_COLOR_RESET " %s%-9s " RLOG_COLOR_RESET, timebuffer,
                            tls_thread_name, severity.color_code, severity.value, domain.color_code, domain_name);
    }
    break;
    case console_log_style::verbose_no_color:
    {
        // prepare timestamp
        char timebuffer[18];
        write_timebuffer(timebuffer, sizeof(timebuffer), "%d.%m.%y %H:%M:%S");

        // full log line
        // [timestamp]     [thread id] [severity] [domain] [message]
        // 06.05.20 07:14:10 t000      INFO       NET      <the message being printed>\n

        auto domain_name = domain.value ? domain.value : "LOG";
        return std::fprintf(stream, "%s %s %-7s  %-9s ", timebuffer, tls_thread_name, severity.value, domain_name);
    }
    break;
    case console_log_style::brief:
    {
        // prepare timestamp
        char timebuffer[9];
        write_timebuffer(timebuffer, sizeof(timebuffer), "%H:%M:%S");

        // brief log line
        // [timestamp] [severity] [domain] [message]
        // 07:14:10 INFO [NET] <the message being printed>\n

        // timestamp and severity (always)
        auto length
            = std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s " RLOG_COLOR_RESET "%s%s " RLOG_COLOR_RESET, timebuffer, severity.color_code, severity.value);

        if (domain.value != nullptr) // domain, optional
            length += std::fprintf(stream, "%s%s " RLOG_COLOR_RESET, domain.color_code, domain.value);

        return length;
    }
    break;
    case console_log_style::briefer:
    {
        // prepare timestamp
        char timebuffer[6];
        write_timebuffer(timebuffer, sizeof(timebuffer), "%H:%M");

        // briefer log line
        // [timestamp] [severity] [domain] [message]
        // 07:14 I [NET] <the message being printed>\n

        // timestamp and severity (always)
        auto length = std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s " RLOG_COLOR_RESET "%s%c " RLOG_COLOR_RESET, timebuffer, severity.color_code,
                                   severity.value[0] ? severity.value[0] : ' ');

        if (domain.value != nullptr) // domain, optional
            length += std::fprintf(stream, "%s%s " RLOG_COLOR_RESET, domain.color_code, domain.value);

        return length;
    }
    break;
    case console_log_style::message_only:
        // intentionally left blank.
        return 0;
    default:
        CC_UNREACHABLE("unsupported log style");
    }
}

void rlog::set_current_thread_name(const char* fmt, ...)
{
    if (fmt != nullptr)
    {
        std::va_list args;
        va_start(args, fmt);
        std::vsnprintf(tls_thread_name, sizeof(tls_thread_name), fmt, args);
        va_end(args);
    }
    else
    {
        tls_thread_name[0] = '\0';
    }
}

void rlog::set_console_log_style(rlog::console_log_style style) { g_log_style = style; }

bool rlog::enable_win32_colors()
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
