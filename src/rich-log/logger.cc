#include "logger.hh"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <clean-core/macros.hh>
#include <clean-core/utility.hh>

#ifdef CC_OS_WINDOWS
#include <clean-core/native/win32_sanitized.hh>
#endif

#define RLOG_COLOR_TIMESTAMP "\u001b[38;5;37m"
#define RLOG_COLOR_RESET "\u001b[0m"

namespace
{
thread_local char tls_thread_name[32] = "";
rlog::console_log_style g_log_style = rlog::console_log_style::verbose;
}

void rlog::print_to_console(severity severity, rlog::domain domain, const char* message, bool use_stderr, bool flush)
{
    auto const stream = use_stderr ? stderr : stdout;

    switch (g_log_style)
    {
    case console_log_style::verbose:
    {
        // prepare timestamp
        std::time_t const t = std::time(nullptr);
        std::tm* const lt = std::localtime(&t);
        char timebuffer[18];
        timebuffer[std::strftime(timebuffer, sizeof(timebuffer), "%d.%m.%y %H:%M:%S", lt)] = '\0';

        // full log line
        // [timestamp]     [thread id] [severity] [domain] [message]
        // 06.05.20 07:14:10 t000      INFO       NET      <the message being printed>\n

        // timestamp and severity (always)
        std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s %s " RLOG_COLOR_RESET "%s%-5s" RLOG_COLOR_RESET, timebuffer, tls_thread_name,
                     severity.color_code, severity.value);

        auto domain_name = domain.value ? domain.value : "LOG";
        std::fprintf(stream, " %s%-9s", domain.color_code, domain_name);

        // message and newline (always)
        std::fprintf(stream, RLOG_COLOR_RESET " %s" RLOG_COLOR_RESET "\n", message);
    }
    break;
    case console_log_style::brief:
    {
        // prepare timestamp
        std::time_t const t = std::time(nullptr);
        std::tm* const lt = std::localtime(&t);
        char timebuffer[9];
        timebuffer[std::strftime(timebuffer, sizeof(timebuffer), "%H:%M:%S", lt)] = '\0';

        // brief log line
        // [timestamp] [severity] [domain] [message]
        // 07:14:10 INFO [NET] <the message being printed>\n

        // timestamp and severity (always)
        std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s " RLOG_COLOR_RESET "%s%-5s" RLOG_COLOR_RESET, timebuffer, severity.color_code, severity.value);

        if (domain.value != nullptr)
        {
            // domain, optional
            std::fprintf(stream, " %s%s", domain.color_code, domain.value);
        }

        // message and newline (always)
        std::fprintf(stream, RLOG_COLOR_RESET " %s" RLOG_COLOR_RESET "\n", message);
    }
    break;
    case console_log_style::briefer:
    {
        // prepare timestamp
        std::time_t const t = std::time(nullptr);
        std::tm* const lt = std::localtime(&t);
        char timebuffer[6];
        timebuffer[std::strftime(timebuffer, sizeof(timebuffer), "%H:%M", lt)] = '\0';

        // briefer log line
        // [timestamp] [severity] [domain] [message]
        // 07:14 I [NET] <the message being printed>\n

        // timestamp and severity (always)
        std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s " RLOG_COLOR_RESET "%s%c" RLOG_COLOR_RESET, timebuffer, severity.color_code,
                     severity.value[0] ? severity.value[0] : ' ');

        if (domain.value != nullptr)
        {
            // domain, optional
            std::fprintf(stream, " %s%s", domain.color_code, domain.value);
        }

        // message and newline (always)
        std::fprintf(stream, RLOG_COLOR_RESET " %s" RLOG_COLOR_RESET "\n", message);
    }
    break;
    case console_log_style::message_only:
    {
        // message and newline (always)
        std::fprintf(stream, RLOG_COLOR_RESET "%s" RLOG_COLOR_RESET "\n", message);
    }
    break;
    default:
        CC_UNREACHABLE("unsupported log style");
    }

    if (flush)
        std::fflush(stream);
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

void rlog::set_console_log_style(rlog::console_log_style style) { g_log_style = style; }
