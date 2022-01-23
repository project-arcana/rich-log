#include "MessageBuilder.hh"

#include <cstdarg>
#include <cstdio>

#include <clean-core/assert.hh>
#include <clean-core/vector.hh>

#include <rich-log/LoggerBase.hh>
#include <rich-log/StdOutLogger.hh>
#include <rich-log/experimental.hh>

using namespace rlog;

static cc::unique_function<bool(cc::string_view domain, cc::string_view msg)> sWhitelistFilter;

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

void MessageBuilder::append_formatted(cc::string_view fmt, cc::span<cc::string const> args)
{
    size_t arg_i = 0;
    for (size_t i = 0; i < fmt.size(); ++i)
    {
        if (i + 1 < fmt.size() && fmt[i] == '{' && fmt[i + 1] == '}')
        {
            CC_ASSERT(arg_i < args.size() && "more escape sequences than provided arguments");
            _msg += args[arg_i];
            ++arg_i;
            ++i;
        }
        else
        {
            _msg += fmt[i];
        }
    }

#ifdef CC_ENABLE_ASSERTIONS
    if (arg_i != args.size())
    {
        std::fprintf(stderr, "[rich-log] provided %zu arguments for just %zu {} escape sequences\n    in log from %s:%d    (function %s)\n",
                     args.size(), arg_i, this->_location.file, this->_location.line, this->_location.function);
    }
#endif

    CC_ASSERT(arg_i == args.size() && "less escape sequences than provided arguments");
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
        logMessageToStdOut(console_log_style::verbose, msgParams, _msg.c_str());
    }
}
