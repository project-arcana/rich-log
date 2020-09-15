#pragma once

#include <clean-core/macros.hh>

#include <rich-log/MessageBuilder.hh>
#include <rich-log/location.hh>

#define RICH_LOG_IMPL(_functor_) ::rlog::MessageBuilder(::rlog::location{CC_PRETTY_FUNC, __FILE__, __LINE__}, _functor_)
