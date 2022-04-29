#pragma once

#include <clean-core/string_view.hh>
#include <clean-core/unique_function.hh>

#include <rich-log/detail/api.hh>

namespace rlog::experimental
{
/// sets a global whitelist filter function
/// if any of these returns true, then messages that must be whitelisted get printed
/// NOTE: this function must be externally synchronized with itself and all logging
RLOG_API void set_whitelist_filter(cc::unique_function<bool(cc::string_view domain, cc::string_view message)> filter);
}
