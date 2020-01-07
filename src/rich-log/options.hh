#pragma once

// TODO: think about non-literal values

namespace rlog
{
// not enum class so that LOG(info) works
enum channel
{
    info,
    warning,
    error,
    debug
};

struct prefix
{
    explicit prefix(char const* v) : value(v) {}

    char const* value;
};

struct sep
{
    explicit sep(char const* v) : value(v) {}

    char const* value;
};

static constexpr struct no_sep_t
{
} no_sep;
}
