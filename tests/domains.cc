#include <nexus/test.hh>

#include <rich-log/log.hh>
#include <rich-log/logger.hh>

RICH_LOG_DECLARE_DOMAIN(Test);

RICH_LOG_DECLARE_DOMAIN(Other);
RICH_LOG_DEFINE_DOMAIN(Other);

TEST("domains")
{
    auto has_default = false;
    auto has_test = false;
    auto has_other = false;

    for (auto d : rlog::get_domains())
    {
        auto name = cc::string_view(d->name);

        if (name == "Default")
            has_default = true;
        else if (name == "Test")
            has_test = true;
        else if (name == "Other")
            has_other = true;
    }

    CHECK(has_default);
    CHECK(has_other);
    CHECK(has_test);
}
