#pragma once

namespace rlog
{
struct location
{
    char const* function;
    char const* file;
    int line;
};
}
