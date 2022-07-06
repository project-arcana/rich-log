#pragma once

#include <cstdint>

#include <clean-core/macros.hh>

#include <rich-log/detail/api.hh>

namespace rlog
{
namespace verbosity
{
/// Verbosity defines how relevant a log message is in increasing order
/// there can be minimum verbosity levels compile-time and run-time
/// per default:
///   - TRACE is disabled compile-time
///   - Debug is disabled run-time
///   - everything else is enabled
enum type : int
{
    _None = 0,

    /// TRACE is used to provide diagnostic information verbosely and in very small steps
    /// e.g. logging in each iteration of an algorithm
    Trace,

    /// DEBUG is used to provide diagnostic information that usually do not matter
    /// e.g. logging the arguments to an algorithm
    Debug,

    /// INFO is used to log interesting information that USERS of your algorithm might want to know about
    /// e.g. the selected graphics card, filenames for large save operations, UI actions
    Info,

    /// WARNING is used to log suspicious calls / inputs
    /// e.g. loading empty files, showing empty meshes, degenerate faces
    /// while they are not strictly errors, you should minimize false positives (users will start to ignore them)
    Warning,

    /// ERRROR is used for clear errors and wrong use of APIs
    /// e.g. trying to load a nonexistant file, providing unsupported argument combinations
    /// these errors do not crash the program, but are clearly wrong
    Error,

    /// FATAL is used for errors that will, with a high probability, crash/invalidate the program
    /// e.g. failure to acquire crucial resources, out of memory
    /// fatal errors also tend to be unrecoverable with no clear way to fix them
    Fatal,

    _EndRange,
    _NumVerbosities = _EndRange - 1
};
}

struct RLOG_API domain_info
{
    rlog::verbosity::type min_verbosity = rlog::verbosity::_None;
    rlog::verbosity::type const compile_time_min_verbosity = rlog::verbosity::_None;

    char const* const name = nullptr;
    char const* const description = nullptr;
    char const* const ansi_color_code = nullptr;
    uint64_t name_hash = 0;

    domain_info(char const* name, char const* desc, char const* ansi_color, rlog::verbosity::type min_verbosity, int compile_min_verbosity);

    static domain_info* get_first_domain();
    static domain_info* find_domain(char const* name);

    domain_info* get_next_domain() const { return next_domain; }

private:
    domain_info(domain_info const&) = delete;
    domain_info(domain_info&&) = delete;

    domain_info* next_domain = nullptr;
};
}

/// declares a logging domain
/// these names are identifiers of _namespaces_
/// so hierarchies can be formed by using ::
///
/// the RICH_LOG_DECLARE_DOMAIN_MV version allows to specify a _compiletime_ min verbosity
/// i.e. all logging calls with less verbosity will not generate code
///     (and, of course, cannot be enabled at runtime)
/// the default is Debug
///
/// Usage:
///
///   RICH_LOG_DECLARE_DOMAIN(MeshImporter::FBX);
///   RICH_LOG_DECLARE_DOMAIN_MV(MeshImporter::FBX, debug);
///
/// Note:
///
///   in some .cc, you need to add:
///   RICH_LOG_DEFINE_DOMAIN(MeshImporter::FBX);
///
///   TODO: how can the struct be custom initialized?
///
///   Domain settings can be changed anytime, e.g.:
///
///     rlog::domains::Default::domain.min_verbosity = rlog::verbosity::Debug;
///     rlog::domains::Default::domain.break_on_log[rlog::verbosity::Warning] = true;
///
///     (this should be externally synchronized, otherwise it might create a race condition)
///
#define RICH_LOG_DECLARE_DOMAIN(Name) RICH_LOG_DECLARE_DOMAIN_EX(Name, Debug, extern)

#define RICH_LOG_DECLARE_DOMAIN_MV(Name, MinVerbosity) RICH_LOG_DECLARE_DOMAIN_EX(Name, MinVerbosity, extern)

#define RICH_LOG_DECLARE_DOMAIN_EX(Name, MinVerbosity, APIPrefix) \
    namespace rlog::domains                                       \
    {                                                             \
    namespace Name                                                \
    {                                                             \
    enum : int                                                    \
    {                                                             \
        CompileTimeMinVerbosity = rlog::verbosity::MinVerbosity   \
    };                                                            \
                                                                  \
    APIPrefix rlog::domain_info domain;                           \
    }                                                             \
    }                                                             \
    CC_FORCE_SEMICOLON


#define RICH_LOG_DEFINE_DOMAIN_EX(Name, Description, ANSIColor, MinRuntimeVerbosity) \
    ::rlog::domain_info rlog::domains::Name::domain(#Name, Description, ANSIColor, ::rlog::verbosity::MinRuntimeVerbosity, rlog::domains::Name::CompileTimeMinVerbosity)

#define RICH_LOG_DEFINE_DOMAIN(Name) RICH_LOG_DEFINE_DOMAIN_EX(Name, nullptr, "\u001b[38;5;244m", Info)

// this namespace is user-extensible
namespace rlog::domains
{
// use RICH_LOG_DECLARE_DOMAIN
//  or RICH_LOG_DECLARE_DOMAIN_MV
//
// e.g. 'RICH_LOG_DECLARE_DOMAIN_MV(My::Domain, debug);' will add:
//
//   namespace My::Domain
//   {
//   enum : int
//   {
//       CompileTimeMinVerbosity = rlog::verbosity::Debug
//   };
//
//   extern domain_info domain;
//   }
}

// declare the default domain
RICH_LOG_DECLARE_DOMAIN_EX(Default, Debug, RLOG_API extern);
