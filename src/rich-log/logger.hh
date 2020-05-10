#pragma once

#include <cstdio>

#include <clean-core/macros.hh>

#include <rich-log/options.hh>

namespace rlog
{
/// prints a formatted, colored prefix to the specified stream, of the form
/// [timestamp]  [thread id]   [severity] [domain]
/// 06.05.20 07:14:10 t000      INFO       NET
void print_prefix_to_stream(severity severity, domain domain, std::FILE* stream = stdout);

/// sets the name for the calling thread, as it appears in the log prefix, using printf syntax
/// pass nullptr to un-set
void set_current_thread_name(char const* fmt, ...) CC_PRINTF_FUNC(1, 2);

/// enables ANSI Escape sequences in Windows conhost.exe and cmd.exe
/// returns true on success
bool enable_win32_colors();
}
