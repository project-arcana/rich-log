#include "domain.hh"

#include <clean-core/assert.hh>
#include <clean-core/stringhash.hh>

// default domain
RICH_LOG_DEFINE_DOMAIN(Default);

namespace
{
rlog::domain_info* gDomainHead = nullptr;
rlog::domain_info* gDomainTail = nullptr;
}

rlog::domain_info::domain_info(char const* name, char const* desc, char const* ansi_color, rlog::verbosity::type min_verbosity, int compile_min_verbosity)
  : min_verbosity(min_verbosity),
    compile_time_min_verbosity(rlog::verbosity::type(compile_min_verbosity)),
    name(name),
    description(desc),
    ansi_color_code(ansi_color),
    name_hash(cc::stringhash(name))
{
    if (gDomainHead)
    {
        // insert self into linked list
        gDomainHead->next_domain = this;
        gDomainHead = this;
    }
    else
    {
        // initialize linked list
        gDomainTail = this;
        gDomainHead = this;
    }
}

rlog::domain_info* rlog::domain_info::get_first_domain() { return gDomainTail; }

rlog::domain_info* rlog::domain_info::find_domain(char const* name)
{
    CC_CONTRACT(name);

    auto const searched_hash = cc::stringhash(name);

    domain_info* cursor = get_first_domain();
    while (cursor)
    {
        if (cursor->name_hash == searched_hash)
        {
            return cursor;
        }

        cursor = cursor->next_domain;
    }

    return nullptr;
}
