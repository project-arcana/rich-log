#include <nexus/test.hh>

#include <rich-log/experimental.hh>
#include <rich-log/log.hh>
#include <rich-log/logger.hh>

TEST("basic logging")
{
    rlog::enable_win32_colors();
    rlog::set_current_thread_name("td#0");

    rlog::experimental::set_whitelist_filter([](cc::string_view domain, cc::string_view) { return domain.empty() || domain.contains("debug"); });

    for (auto style : {rlog::console_log_style::verbose, rlog::console_log_style::brief, rlog::console_log_style::briefer, rlog::console_log_style::message_only})
    {
        rlog::set_console_log_style(style);

        {
            LOG("format {}", 5);

            int x = 3;
            LOG << 1 << "hello" << true;
            LOG() << x << "goodbye" << false;

            LOG_INFO() << 1.f;
            LOG_INFO("test {}", "test");


            LOG_DEBUG << 2u;
            LOG_DEBUG_DOMAIN("my_debug") << 112u;
            LOG_DEBUG_DOMAIN("not whitelisted") << 442u;
            LOG_WARN << 3.0;
            LOG_ERROR << '5';

            LOG("bla: {} {}", 7, true);
            LOG("test: {}", 7) << 7 + 2;
            //                LOG_EXPR(1 + x);
            //                LOG_EXPR(2 + x, error);
        }
    }

    //    {
    //        constexpr auto const custom_domain = rlog::domain("MYDOMAIN");

    //        auto const custom_log = [&](auto... additional) { return rlog::MessageBuilder(custom_domain, additional...); };

    //        custom_log()("test");
    //        custom_log(rlog::err_out)("test with error");
    //    }

    CHECK(true); // disable warning
}
