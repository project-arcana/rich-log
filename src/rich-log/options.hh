#pragma once

// TODO: think about non-literal values

namespace rlog
{
struct severity
{
    explicit constexpr severity(char const* v, char const* color = "") : value(v), color_code(color) {}
    char const* value;      ///< Name of the severity
    char const* color_code; ///< ANSI color codes

    static constexpr severity trace() { return severity{"TRACE", "\u001b[38;5;14m"}; }
    static constexpr severity info() { return severity{"INFO", "\u001b[38;5;241m"}; }
    static constexpr severity debug() { return severity{"DEBUG", "\u001b[38;5;148m"}; }
    static constexpr severity warning() { return severity{"WARN", "\u001b[38;5;202m"}; }
    static constexpr severity error() { return severity{"ERROR", "\u001b[38;5;196m\u001b[1m"}; }
};
struct domain
{
    explicit constexpr domain(char const* v, char const* color = "\u001b[38;5;244m") : value(v), color_code(color) {}
    char const* value;      ///< Name of the domain
    char const* color_code; ///< ANSI color codes

    static constexpr domain unspecified() { return domain{"LOG", "\u001b[38;5;238m"}; }
};
}
