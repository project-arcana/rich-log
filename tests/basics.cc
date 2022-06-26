#include <nexus/test.hh>

#include <rich-log/experimental.hh>
#include <rich-log/log.hh>
#include <rich-log/logger.hh>

RICH_LOG_DECLARE_DOMAIN(Test);
RICH_LOG_DEFINE_DOMAIN(Test);

TEST("basic logging", disabled) // call directly to run this test (it will print to console)
{
    rlog::enable_win32_colors();
    rlog::set_current_thread_name("td#0");

    LOG("hello world!");
    LOG("{1} {0}!", "world", "hello");

    LOGD(Test, Warning, "'tis a warning, mate");
    LOGD(Test, Info, "information follows at %s", 12345);

    LOGD(Test, Error, "an error occured"); // debugger should break here

    CHECK(true); // disable warning
}

TEST("logging capture")
{
    cc::string msg;
    auto _ = rlog::scoped_logger_override(
        [&](rlog::message_ref m, bool& do_break)
        {
            msg = m.message;
            do_break = false;
        });

    LOG("Hello World");
    CHECK(msg == "Hello World");

    LOGD(Test, Warning, "warning nr %d", 10);
    CHECK(msg == "warning nr 10");

    LOGD(Test, Fatal, "%% fatal %%");
    CHECK(msg == "% fatal %");

    {
        cc::string msg2;
        auto _ = rlog::scoped_logger_override([&](rlog::message_ref m, bool&) { msg2 = m.message; });

        LOG("nested msg");
        CHECK(msg == "% fatal %");
        CHECK(msg2 == "nested msg");
    }

    LOG("not nested anymore");
    CHECK(msg == "not nested anymore");
}

TEST("default logger")
{
    cc::string msg;
    rlog::set_global_default_logger([&](rlog::message_ref m, bool&) { msg = m.message; });

    LOG("message A");
    CHECK(msg == "message A");

    {
        cc::string msg2;
        auto _ = rlog::scoped_logger_override([&](rlog::message_ref m, bool&) { msg2 = m.message; });

        LOG("local msg");
        CHECK(msg == "message A");
        CHECK(msg2 == "local msg");
    }

    LOG("not nested anymore");
    CHECK(msg == "not nested anymore");

    rlog::set_global_default_logger({});
}
