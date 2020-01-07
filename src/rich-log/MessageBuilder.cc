#include "MessageBuilder.hh"

#include <cstdio>

using namespace rlog;

MessageBuilder::~MessageBuilder()
{
    // TODO: send message to logger

    // DEBUG: console for now

    auto const stream = _channel == warning || _channel == error ? stderr : stdout;

    cc::string m;
    switch (_channel)
    {
    case info:
        m += "[info]";
        break;
    case warning:
        m += "[warn]";
        break;
    case error:
        m += "[error]";
        break;
    case debug:
        m += "[debug]";
        break;
    default:
        m += "[unknown]";
        break;
    }
    m += ' ';
    m += _msg;
    m += '\n';
    fputs(m.c_str(), stream);
}
