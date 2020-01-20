#include "MessageBuilder.hh"

#include <cstdarg>
#include <cstdio>

#include <clean-core/assert.hh>

using namespace rlog;

void MessageBuilder::append_formatted(cc::string_view fmt, cc::span<cc::string const> args)
{
    size_t arg_i = 0;
    for (size_t i = 0; i < fmt.size(); ++i)
    {
        if (i + 1 < fmt.size() && fmt[i] == '{' && fmt[i + 1] == '}')
        {
            CC_ASSERT(arg_i < args.size());
            _msg += args[arg_i];
            ++arg_i;
            ++i;
        }
        else
        {
            _msg += fmt[i];
        }
    }
}

MessageBuilder::~MessageBuilder()
{
    // TODO: send message to logger
    // DEBUG: console for now

    auto const stream = _use_err_stream ? stderr : stdout;

    cc::string m;
    m.reserve(_msg.size() + 1 + std::strlen(_prefix));
    m += _prefix;
    m += _msg;
    m += '\n';

    std::fputs(m.c_str(), stream);
    std::fflush(stream);
}
