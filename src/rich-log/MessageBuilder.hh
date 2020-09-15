#pragma once

#include <clean-core/forward.hh>
#include <clean-core/function_ptr.hh>
#include <clean-core/span.hh>
#include <clean-core/string.hh>

#include <rich-log/fwd.hh>
#include <rich-log/location.hh>
#include <rich-log/options.hh>

#include <reflector/to_string.hh>

namespace rlog
{
// TODO: get a big temporary buffer, write the message into the buffer, then copy into actual msg
class MessageBuilder
{
    // options
public:
    constexpr void set_severity(severity const& s) { _severity = s; }
    constexpr void set_domain(domain const& d) { _domain = d; }
    constexpr void set_separator(char const* s) { _sep = s; }
    constexpr void set_use_error_stream(bool enabled) { _use_err_stream = enabled; }
    void set_location(location const& loc) { _location = loc; }

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
    MessageBuilder() = default;

    MessageBuilder(location const& location, cc::function_ptr<void(MessageBuilder&)> functor = nullptr) : _location(location)
    {
        if (functor)
            functor(*this);
    }

    template <class... Args>
    void format(char const* fmt, Args const&... args)
    {
        if constexpr (sizeof...(Args) == 0)
            append(fmt);
        else
        {
            cc::string args_s[] = {rf::to_string(args)...};
            append_formatted(fmt, args_s);
        }
    }

    MessageBuilder& operator()() { return *this; }

    template <class... Args>
    MessageBuilder& operator()(char const* fmt, Args&&... args)
    {
        format(fmt, cc::forward<Args>(args)...);
        return *this;
    }

    template <class... Args>
    MessageBuilder& operator()(cc::function_ptr<void(MessageBuilder&)> functor, char const* fmt, Args&&... args)
    {
        functor(*this);
        format(fmt, cc::forward<Args>(args)...);
        return *this;
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
    location _location = {};
    domain _domain = domain::unspecified();
    severity _severity = severity::info();
    char const* _sep = " ";

    // TODO: some cleverly pooled buffer structure
    cc::string _msg;

    bool _use_err_stream = false;
};
}
