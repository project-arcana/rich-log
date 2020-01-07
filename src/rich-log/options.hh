#pragma once

// TODO: think about non-literal values

namespace rlog
{
struct prefix
{
    explicit constexpr prefix(char const* v) : value(v) {}

    char const* value;
};

struct sep
{
    explicit constexpr sep(char const* v) : value(v) {}

    char const* value;
};

static constexpr struct no_sep_t
{
} no_sep;

static constexpr struct err_out_t
{
} err_out;

// preconfigured channels
static constexpr struct info_t
{
} info;
static constexpr struct warning_t
{
} warning;
static constexpr struct error_t
{
} error;
static constexpr struct debug_t
{
} debug;

}