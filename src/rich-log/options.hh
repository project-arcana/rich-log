#pragma once

// TODO: think about non-literal values

namespace rlog
{
struct severity
{
    explicit constexpr severity(char const* v, char const* color = "") : value(v), color_code(color) {}
    char const* value;      ///< Name of the severity
    char const* color_code; ///< ANSI color codes, default: empty

    static constexpr severity trace() { return severity{"TRACE", "\u001b[38;5;14m"}; }
    static constexpr severity info() { return severity{"INFO", "\u001b[38;5;241m"}; }
    static constexpr severity debug() { return severity{"DEBUG", "\u001b[38;5;148m"}; }
    static constexpr severity warning() { return severity{"WARN", "\u001b[38;5;202m"}; }
    static constexpr severity error() { return severity{"ERROR", "\u001b[38;5;196m\u001b[1m"}; }
};
struct domain
{
    explicit constexpr domain(char const* v, char const* color = "\u001b[38;5;241m") : value(v), color_code(color) {}
    char const* value;      ///< Name of the domain, nullptr is valid
    char const* color_code; ///< ANSI color codes, default: empty

    static constexpr domain null() { return domain{nullptr}; }
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
