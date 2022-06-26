#pragma once

#include <clean-core/macros.hh>

namespace rlog
{
namespace verbosity
{
enum type : int
{
    /// TRACE is used to provide diagnostic information verbosely and in very small steps
    /// e.g. logging in each iteration of an algorithm
    /// per default, trace should be compile-time DISABLED (as it can and will slow down the app)
    Trace = 0,

    /// DEBUG is used to provide diagnostic information that usually do not matter
    /// e.g. logging the arguments to an algorithm
    /// per default, debug should be compile-time ENABLED but runtime DISABLED
    /// this means all message can be viewed post-mortem
    Debug = 1,

    /// INFO is used to log interesting information that USERS of your algorithm might want to know about
    /// e.g. the selected graphics card, filenames for large save operations, UI actions
    /// per default, this logging is enabled
    Info = 2,

    /// WARNING is used to log suspicious calls / inputs
    /// e.g. loading empty files, showing empty meshes, degenerate faces
    /// while they are not strictly errors, you should minimize false positives (users will start to ignore them)
    /// per default, this logging is enabled
    Warning = 3,

    /// ERRROR is used for clear errors and wrong use of APIs
    /// e.g. trying to load a nonexistant file, providing unsupported argument combinations
    /// these errors do not crash the program, but are clearly wrong
    /// per default, this logging is enabled, will break, and will record stacktraces
    Error = 4,

    /// FATAL is used for errors that will, with a high probability, crash/invalidate the program
    /// e.g. failure to acquire crucial resources, out of memory
    /// fatal errors also tend to be unrecoverable with no clear way to fix them
    /// per default, this logging is enabled, will break, and will record stacktraces
    Fatal = 5,

    _count
};
}

struct domain_info
{
    rlog::verbosity::type verbosity = rlog::verbosity::Info; // always first
    char const* name = "";
    char const* ansi_color_code = "\u001b[38;5;244m";
    bool break_on_log[verbosity::_count] = {false, false, false, false, true, true};
    bool record_stacktrace[verbosity::_count] = {false, false, false, false, true, true};

    static constexpr domain_info make_named(char const* name)
    {
        domain_info di;
        di.name = name;
        return di;
    }
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
#define RICH_LOG_DECLARE_DOMAIN(Name) RICH_LOG_DECLARE_DOMAIN_MV(Name, Debug)
#define RICH_LOG_DECLARE_DOMAIN_MV(Name, min_verbosity)          \
    namespace rlog::domains                                      \
    {                                                            \
    namespace Name                                               \
    {                                                            \
    enum : int                                                   \
    {                                                            \
        CompileTimeMinVerbosity = rlog::verbosity::min_verbosity \
    };                                                           \
                                                                 \
    extern rlog::domain_info domain;                             \
    }                                                            \
    }                                                            \
    CC_FORCE_SEMICOLON
#define RICH_LOG_DEFINE_DOMAIN(Name) ::rlog::domain_info rlog::domains::Name::domain = ::rlog::domain_info::make_named(#Name) // force ;

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

// declare a default domain
RICH_LOG_DECLARE_DOMAIN(Default);
