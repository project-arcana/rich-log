#include <nexus/test.hh>

#include <rich-log/experimental.hh>
#include <rich-log/log.hh>
#include <rich-log/logger.hh>

TEST("basic logging")
{
    rlog::enable_win32_colors();
    rlog::set_current_thread_name("td#0");

    LOG("hello world!");
    LOG("{1} {0}!", "world", "hello");

    CHECK(true); // disable warning
}
