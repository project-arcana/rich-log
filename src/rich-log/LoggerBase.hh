#pragma once

#include <rich-log/detail/api.hh>
#include <rich-log/fwd.hh>

namespace rlog
{
class RLOG_API LoggerBase
{
public:
    explicit LoggerBase(char const* name);

    virtual void log(message_params const& params, char const* msg) = 0;

    LoggerBase* getPrev() const { return mPrev; }
    LoggerBase* getNext() const { return mNext; }
    char const* getName() const { return mName; }

    virtual ~LoggerBase();

private:
    LoggerBase(LoggerBase const&) = delete;
    LoggerBase(LoggerBase&&) = delete;
    LoggerBase& operator=(LoggerBase const&) = delete;
    LoggerBase& operator=(LoggerBase&&) = delete;

    LoggerBase* mPrev = nullptr;
    LoggerBase* mNext = nullptr;
    char const* mName = nullptr;
};

// broadcast a log message to all registered loggers
RLOG_API int broadcastMessage(message_params const& params, char const* msg);
}
