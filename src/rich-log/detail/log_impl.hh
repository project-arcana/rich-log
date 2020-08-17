#pragma once

#include <clean-core/macros.hh>

#include <rich-log/MessageBuilder.hh>
#include <rich-log/location.hh>

#define RICH_LOG_IMPL(_functor_)                                                            \
    [] {                                                                                    \
        static constexpr ::rlog::location _rlog_loc = {CC_PRETTY_FUNC, __FILE__, __LINE__}; \
        return ::rlog::MessageBuilder(&_rlog_loc, _functor_);                               \
    }()
