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
}

void rlog::print_prefix_to_stream(rlog::severity severity, rlog::domain domain, std::FILE* stream)
{
    // prepare timestamp
    std::time_t const t = std::time(nullptr);
    std::tm* const lt = std::localtime(&t);
    char timebuffer[18];
    timebuffer[std::strftime(timebuffer, sizeof(timebuffer), "%d.%m.%y %H:%M:%S", lt)] = '\0';

    // full log line
    // [timestamp]     [thread id] [severity] [domain] [message]
    // 06.05.20 07:14:10 t000      INFO       NET      <the message being printed>\n
    // the prefix is everything up to [message]

    std::fprintf(stream, RLOG_COLOR_TIMESTAMP "%s %s " RLOG_COLOR_RESET "%s%-7s " RLOG_COLOR_RESET " %s%-9s " RLOG_COLOR_RESET, timebuffer,
                 tls_thread_name, severity.color_code, severity.value, domain.color_code, domain.value);
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
