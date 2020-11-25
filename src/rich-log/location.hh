#pragma once

#include <clean-core/macros.hh>

namespace rlog
{
// NOTE: std::source_location can eventually replace this
// which also removes the need for macros
struct location
{
    char const* function;
    char const* file;
    int line;
};

#define RLOG_LOCATION() ::rlog::location{CC_PRETTY_FUNC, __FILE__, __LINE__}
}
