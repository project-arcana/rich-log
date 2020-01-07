#pragma once

#include <clean-core/forward.hh>
#include <clean-core/string.hh>

#include <rich-log/fwd.hh>
#include <rich-log/options.hh>

#include <reflector/to_string.hh>

namespace rlog
{
// TODO: get a big temporary buffer, write the message into the buffer, then copy into actual msg
class MessageBuilder
{
    // options
private:
    void configure(prefix const& p) { _prefix = p.value; }
    void configure(channel c) { _channel = c; }
    void configure(sep const& s) { _sep = s.value; }
    void configure(no_sep_t) { _sep = ""; }

    // formatted message
public:
    template <class... Args>
    MessageBuilder& operator()(char const* fmt, Args&&... args)
    {
        // TODO
        return *this;
    }

    // object log
public:
    template <class T>
    MessageBuilder& operator<<(T const& value)
    {
        // TODO: some structured alternative?
        append(rf::to_string(value));
        return *this;
    }

public:
    template <class... Args>
    MessageBuilder(rlog::location const& loc, Args&&... args) : _location(loc)
    {
        // configure message
        ((this->configure(cc::forward<Args>(args))), ...);
    }

    MessageBuilder(MessageBuilder const&) = delete;
    MessageBuilder(MessageBuilder&&) = delete;
    MessageBuilder& operator=(MessageBuilder const&) = delete;
    MessageBuilder& operator=(MessageBuilder&&) = delete;

    ~MessageBuilder();

private:
    void append(cc::string_view s)
    {
        if (!_msg.empty())
            _msg += _sep;
        _msg += s;
    }

private:
    location const& _location;
    channel _channel = info;
    char const* _prefix = "";
    char const* _sep = ", ";

    // TODO: some cleverly pooled buffer structure
    cc::string _msg;
};
}
