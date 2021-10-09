#pragma once

#include <cstdint>

namespace rlog
{
enum class color_style : uint32_t;
enum class severity : uint32_t;

struct location;
struct domain;
struct message_params;

class MessageBuilder;
class LoggerBase;
class StdOutLogger;
}
