#pragma once

#include <cstdint>

#include <clean-core/macros.hh>

#include <rich-log/fwd.hh>

namespace rlog
{
struct location
{
    char const* function;
    char const* file;
    int line;
};

#define RLOG_LOCATION() \
    ::rlog::location { CC_PRETTY_FUNC, __FILE__, __LINE__ }

//// values for 256bit ANSI escape colors
//// format: ESC[38;5;NUMBER
//// ie "\u001b[38;5;148m" for tbl_lightgreen (foreground)
// enum class ansi_color
//{
//    // standard colors (0-7)
//    std_black = 0,
//    std_red,
//    std_green,
//    std_yellow,
//    std_blue,
//    std_magenta,
//    std_cyan,
//    std_gray,
//
//    // high-intensity colors (8-15)
//    hi_gray = 8,
//    hi_red,
//    hi_green,
//    hi_yellow,
//    hi_blue,
//    hi_magenta,
//    hi_cyan,
//    hi_white,
//
//    // 216 color table (16-231) (selection)
//    tbl_lightgreen = 148,
//    tbl_crimson = 196,
//    tbl_orangered = 202,
//
//    // grayscale (232-255) (selection)
//    gs_black = 232,
//    gs_darkgray = 238,
//    gs_midgray = 244,
//    gs_white = 255
//};

enum class color_style : uint32_t
{
    neutral,

    light_gray,
    dark_gray,
    cyan,
    light_green,
    subtle_blue,
    orangered,
    error_red,
    critical_red,
};

enum class severity : uint32_t
{
    info,
    trace,
    debug,
    warning,
    error,
    critical
};

constexpr color_style getSeverityStyle(severity severity)
{
    switch (severity)
    {
    case severity::info:
        return color_style::light_gray;
    case severity::trace:
        return color_style::cyan;
    case severity::debug:
        return color_style::light_green;
    case severity::warning:
        return color_style::orangered;
    case severity::error:
        return color_style::error_red;
    case severity::critical:
        return color_style::critical_red;
    default:
        return color_style::neutral;
    }
}

constexpr char const* getSeverityName(severity severity)
{
    switch (severity)
    {
    case severity::info:
        return "INFO";
    case severity::trace:
        return "TRACE";
    case severity::debug:
        return "DEBUG";
    case severity::warning:
        return "WARN";
    case severity::error:
        return "ERROR";
    case severity::critical:
        return "CRITICAL";
    default:
        return "";
    }
}

constexpr char const* getColorANSICode(color_style style)
{
    // foreground color codes for 256bit mode
    // ESC[38;5;<NUMBER>m
    switch (style)
    {
    case rlog::color_style::neutral:
        return "";
    case rlog::color_style::light_gray:
        return "\u001b[38;5;244m";
    case rlog::color_style::dark_gray:
        return "\u001b[38;5;238m";
    case rlog::color_style::cyan:
        return "\u001b[38;5;14m";
    case rlog::color_style::light_green:
        return "\u001b[38;5;148m";
    case rlog::color_style::subtle_blue:
        return "\u001b[38;5;37m";
    case rlog::color_style::orangered:
        return "\u001b[38;5;202m";
    case rlog::color_style::error_red:
        return "\u001b[38;5;196m\u001b[1m";
    case rlog::color_style::critical_red:
        return "\u001b[38;5;197m\u001b[1m\u001b[7m";
    default:
        return "";
    }
}

constexpr uint32_t getColorHex(color_style style)
{
    switch (style)
    {
    case rlog::color_style::neutral:
        return 0xFFFFFFFF;
    case rlog::color_style::light_gray:
        return 0xFF808080;
    case rlog::color_style::dark_gray:
        return 0xFF444444;
    case rlog::color_style::cyan:
        return 0xFFFFFF00;
    case rlog::color_style::light_green:
        return 0xFF00D6AF;
    case rlog::color_style::subtle_blue:
        return 0xFFAFAF00;
    case rlog::color_style::orangered:
        return 0xFF005FFF;
    case rlog::color_style::error_red:
        return 0xFF0000FF;
    case rlog::color_style::critical_red:
        return 0xFF5F00FF;
    default:
        return 0xFFFFFFFF;
    }
}

struct domain
{
    explicit constexpr domain(char const* v, color_style color = color_style::light_gray) : name(v), color(color) {}

    char const* name;
    color_style color;

    static constexpr domain unspecified() { return domain{nullptr, color_style::dark_gray}; }
};

struct message_params
{
    rlog::location location;
    rlog::severity severity;
    rlog::domain domain;
};
}
