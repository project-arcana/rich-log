#pragma once

#include <rich-log/options.hh>

namespace rlog
{
/// prints a formatted message to stdout/stderr
void print_to_console(severity severity, domain domain, char const* message, bool use_stderr, bool flush);

/// sets the name for the calling thread, as it appears in the log prefix, using printf syntax
/// pass nullptr to un-set
void set_current_thread_name(char const* fmt, ...);

/// enables ANSI Escape sequences in Windows conhost.exe and cmd.exe
/// returns true on success
bool enable_win32_colors();
}
