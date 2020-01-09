#include "MessageBuilder.hh"

#include <cstdarg>
#include <cstdio>

using namespace rlog;

MessageBuilder& MessageBuilder::operator()(const char* fmt, ...)
{
    if (fmt != nullptr)
    {
        char formatted[1024];
        {
            va_list args;
            va_start(args, fmt);
            std::vsnprintf(formatted, 1024, fmt, args);
            va_end(args);
        }

        append(formatted);
    }

    return *this;
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
