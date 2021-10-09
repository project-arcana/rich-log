#include "LoggerBase.hh"

#include <clean-core/assert.hh>

namespace
{
rlog::LoggerBase* gLoggersHead = nullptr;
rlog::LoggerBase* gLoggersTail = nullptr;
}

rlog::LoggerBase::LoggerBase(char const* name)
{
    CC_CONTRACT(name);

    if (gLoggersHead)
    {
        // insert self into linked list
        mPrev = gLoggersHead;
        gLoggersHead->mNext = this;
        gLoggersHead = this;
    }
    else
    {
        // initialize linked list
        gLoggersTail = this;
        gLoggersHead = this;
    }
}

rlog::LoggerBase::~LoggerBase()
{
    // remove self from linked list
    if (mPrev)
    {
        // this is not the first logger in line
        // situation: [Tail]->[..]->[..]->[mPrev]->[this]->[mNext]->[..]->[..]->[Head]

        CC_ASSERT(mPrev->mNext == this);
        mPrev->mNext = this->mNext;

        if (this->mNext)
        {
            CC_ASSERT(mNext->mPrev == this);
            mNext->mPrev = this->mPrev;
        }
        else
        {
            // this was the head
            CC_ASSERT(gLoggersHead == this);
            gLoggersHead = this->mPrev;
        }
    }
    else
    {
        // this is the first logger in line
        // situation: [Tail == this]->[mNext]->[..]->[..]->[Head]

        CC_ASSERT(gLoggersTail == this);
        gLoggersTail = this->mNext;

        if (this->mNext)
        {
            CC_ASSERT(mNext->mPrev == this);
            mNext->mPrev = nullptr;
        }
        else
        {
            // no loggers remain
            CC_ASSERT(gLoggersHead == this);
            gLoggersHead = nullptr;
        }
    }
}

int rlog::broadcastMessage(message_params const& params, char const* msg)
{
    LoggerBase* logger = gLoggersTail;
    int numLoggers = 0;

    while (logger)
    {
        ++numLoggers;
        logger->log(params, msg);
        logger = logger->getNext();
    }

    return numLoggers;
}
