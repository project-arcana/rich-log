#include "MessageBuilder.hh"

#include <clean-core/assert.hh>
#include <clean-core/vector.hh>

#include <rich-log/logger.hh>

using namespace rlog;

static cc::vector<cc::unique_function<bool(cc::string_view domain, cc::string_view msg)>> sWhitelistFilters;

void rlog::add_whitelist_filter(cc::unique_function<bool(cc::string_view, cc::string_view)> filter) { sWhitelistFilters.push_back(cc::move(filter)); }

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
    {
        auto ok = false;
        for (auto const& f : sWhitelistFilters)
            if (f(_domain.value ? _domain.value : "", _msg))
            {
                ok = true;
                break;
            }
        if (!ok)
            return; // was not whitelisted
    }

    // TODO: send message to logger
    // DEBUG: console for now
    auto* const stream = _use_err_stream ? stderr : stdout;
    print_prefix_to_stream(_severity, _domain, stream);
    std::fprintf(stream, "%s\n", _msg.c_str());
    std::fflush(stream);
}
