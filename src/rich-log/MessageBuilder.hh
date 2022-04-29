#pragma once

#include <clean-core/forward.hh>
#include <clean-core/function_ptr.hh>
#include <clean-core/span.hh>
#include <clean-core/string.hh>

#include <reflector/to_string.hh>

#include <rich-log/detail/CustomPrintf.hh>
#include <rich-log/detail/api.hh>
#include <rich-log/fwd.hh>
#include <rich-log/options.hh>

namespace rlog
{
// TODO: get a big temporary buffer, write the message into the buffer, then copy into actual msg
class RLOG_API MessageBuilder
{
    // options
public:
    void set_severity(severity s) { _severity = s; }
    void set_domain(domain const& d) { _domain = d; }
    void set_separator(char const* s) { _sep = s; }
    void set_must_be_whitelisted(bool enabled) { _must_be_whitelisted = enabled; }
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

    // custom printf formatting - use %v as an universal specifier for anything, including custom types
    // other printf specifiers can be used as usual
    // {} is supported for backwards compatibility
    template <class... Args>
    int format(char const* fmt, Args&&... args)
    {
        char buf[1024];
        int const len = detail::CustomSnprintf<Args...>(buf, sizeof(buf), fmt, cc::forward<Args>(args)...);
        append(cc::string_view(buf, len));
        return len;
    }

    // use (non-custom) printf directly
    void printf(char const* fmt, ...) CC_PRINTF_FUNC(2);

    // use (non-custom) vprintf directly, 'vlist' is va_list
    void vprintf(char const* fmt, char* vlist);

    MessageBuilder& operator()() { return *this; }

    // main usage
    template <class... Args>
    MessageBuilder& operator()(char const* fmt, Args&&... args)
    {
        this->format<Args...>(fmt, cc::forward<Args>(args)...);
        return *this;
    }

    // configuration functor syntax
    // 'functor' modifies the state of this message builder
    template <class... Args>
    MessageBuilder& operator()(cc::function_ptr<void(MessageBuilder&)> functor, char const* fmt, Args&&... args)
    {
        functor(*this);
        this->format<Args...>(fmt, cc::forward<Args>(args)...);
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
    severity _severity = severity::info;
    char const* _sep = " ";

    // TODO: some cleverly pooled buffer structure
    cc::string _msg;

    bool _must_be_whitelisted = false;
};
}
