#include "MessageBuilder.hh"

#include <cstdarg>
#include <cstdio>

#include <clean-core/assert.hh>
#include <clean-core/vector.hh>

#include <rich-log/LoggerBase.hh>
#include <rich-log/StdOutLogger.hh>
#include <rich-log/experimental.hh>

using namespace rlog;

namespace
{
// not sure if we want this configurable since the mechanism for that is custom loggers already
rlog::console_log_style sDefaultLogStyle = console_log_style::brief;

cc::unique_function<bool(cc::string_view domain, cc::string_view msg)> sWhitelistFilter;
}

void rlog::experimental::set_whitelist_filter(cc::unique_function<bool(cc::string_view, cc::string_view)> filter)
{
    sWhitelistFilter = cc::move(filter);
}

void MessageBuilder::printf(const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void rlog::MessageBuilder::vprintf(char const* fmt, char* vlist)
{
    static_assert(std::is_same_v<std::va_list, char*>, "Unexpected va_list type");

    char buf[1024];
    int const num_written = std::vsnprintf(buf, sizeof(buf), fmt, vlist);
    CC_ASSERT(num_written < int(sizeof(buf)) && "MessageBuilder::printf truncated");
    append({buf, size_t(num_written)});
}

MessageBuilder::~MessageBuilder()
{
    if (_must_be_whitelisted)
        if (!sWhitelistFilter || !sWhitelistFilter(_domain.name ? _domain.name : "", _msg))
            return; // was not whitelisted

    auto const msgParams = rlog::message_params{_location, _severity, _domain};
    auto const numLoggers = rlog::broadcastMessage(msgParams, _msg.c_str());

    if (numLoggers == 0)
    {
        // fall back to stdout logger
        logMessageToStdOut(sDefaultLogStyle, msgParams, _msg.c_str());
    }
}
