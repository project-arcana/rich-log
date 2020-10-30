#include "MessageBuilder.hh"

#include <cstdarg>
#include <cstdio>

#include <clean-core/assert.hh>
#include <clean-core/vector.hh>

#include <rich-log/experimental.hh>
#include <rich-log/logger.hh>

using namespace rlog;

static cc::unique_function<bool(cc::string_view domain, cc::string_view msg)> sWhitelistFilter;

void rlog::experimental::set_whitelist_filter(cc::unique_function<bool(cc::string_view, cc::string_view)> filter)
{
    sWhitelistFilter = cc::move(filter);
}

void MessageBuilder::printf(const char* fmt, ...)
{
    char buf[1024];

    std::va_list args;
    va_start(args, fmt);
    int const num_written = std::vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

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
    CC_ASSERT(arg_i == args.size() && "less escape sequences than provided arguments");
}

MessageBuilder::~MessageBuilder()
{
    if (_must_be_whitelisted)
        if (!sWhitelistFilter || !sWhitelistFilter(_domain.value ? _domain.value : "", _msg))
            return; // was not whitelisted

    // TODO: send message to logger
    // DEBUG: console for now
    auto* const stream = _use_err_stream ? stderr : stdout;
    print_prefix_to_stream(_severity, _domain, stream);
    std::fprintf(stream, "%s\n", _msg.c_str());
    std::fflush(stream);
}
