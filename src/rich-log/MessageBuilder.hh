#pragma once

#include <clean-core/forward.hh>
#include <clean-core/span.hh>
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
public:
    void configure(prefix const& p) { _prefix = p.value; }
    void configure(sep const& s) { _sep = s.value; }
    void configure(no_sep_t) { _sep = ""; }
    void configure(err_out_t) { _use_err_stream = true; }
    void configure(location const& loc) { _location = &loc; }

    void configure(info_t) { _prefix = "[info] "; }
    void configure(warning_t)
    {
        _prefix = "[warn] ";
        _use_err_stream = true;
    }
    void configure(error_t)
    {
        _prefix = "[error] ";
        _use_err_stream = true;
    }
    void configure(debug_t) { _prefix = "[debug] "; }

    // formatted message
public:
    template <class... Args>
    MessageBuilder& operator()(char const* fmt, Args const&... args)
    {
        if constexpr (sizeof...(Args) == 0)
            append(fmt);
        else
        {
            cc::string args_s[] = {rf::to_string(args)...};
            append_formatted(fmt, args_s);
        }

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
    MessageBuilder(Args&&... args)
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

    void append_formatted(cc::string_view fmt, cc::span<cc::string const> args);

private:
    location const* _location = nullptr;
    char const* _prefix = "";
    char const* _sep = ", ";

    // TODO: some cleverly pooled buffer structure
    cc::string _msg;

    bool _use_err_stream = false;
};
}
