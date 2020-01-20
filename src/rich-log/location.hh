#pragma once

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
}
