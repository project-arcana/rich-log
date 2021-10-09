#pragma once

#include <rich-log/MessageBuilder.hh>
#include <rich-log/options.hh>

#define RICH_LOG_IMPL(_functor_) ::rlog::MessageBuilder(RLOG_LOCATION(), _functor_)
