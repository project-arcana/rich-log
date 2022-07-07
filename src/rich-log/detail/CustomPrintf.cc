#include "CustomPrintf.hh"

#include <clean-core/assert.hh>

// decorate function names to not pollute symbols if we get linked statically
#define STB_SPRINTF_DECORATE(name) rich_log_stbsp_##name
// implementation in this TU
#define STB_SPRINTF_IMPLEMENTATION
#include <rich-log/lib/stb_sprintf.h>

namespace
{
constexpr bool IsPrintfSpecifierPrefix(char c)
{
    /*
        Characters that could be used for the generic specifier
        Text behind the line means it has some usage in printf specifiers

        aA double hex
        bB (EXT: used in stb sprintf, possibly gcc)
        c char
        d int
        eE e notation double
        fF double
        gG auto e notation double
        h length: short promotion/char promotion
        iI int
        j length: intmax_t
      - k
        lL length: long
      - m
        n write num chars to ptr
        o octal int
        p void*
        q EXT length: quad word (64bit), common on bsd
      - r
        s string
        t length: ptrdiff_t
        u unsigned
      - v
      - w
        xX hex uint
      - y
        z length: size_t
        0-9 length prefix
        [space] flag spacing
        # flag 0x
        + flag
        - flag
        ' EXT flag thousands sep
        * width field
        . precision field
      - ?
        $ (EXT: parameter field but requires a number first)
      - =
        ~
        _
        :
        ;
        ,
    */

    for (auto p : "aAbBcdeEfFgGhiIjlLnopqstuxXz0123456789 #+-.'*")
        if (p == c)
            return true;

    return false;
}
}

namespace rlog::detail
{
static_assert(HasTypePrintfSupport<bool>);
static_assert(HasTypePrintfSupport<char>);
static_assert(HasTypePrintfSupport<signed char>);
static_assert(HasTypePrintfSupport<signed short>);
static_assert(HasTypePrintfSupport<signed int>);
static_assert(HasTypePrintfSupport<signed long>);
static_assert(HasTypePrintfSupport<signed long long>);
static_assert(HasTypePrintfSupport<unsigned char>);
static_assert(HasTypePrintfSupport<unsigned short>);
static_assert(HasTypePrintfSupport<unsigned int>);
static_assert(HasTypePrintfSupport<unsigned long>);
static_assert(HasTypePrintfSupport<unsigned long long>);
static_assert(HasTypePrintfSupport<float>);
static_assert(HasTypePrintfSupport<double>);
static_assert(HasTypePrintfSupport<int*>);
static_assert(HasTypePrintfSupport<void*>);
static_assert(HasTypePrintfSupport<nullptr_t>);
}

namespace
{
// replaces all generic specifiers %k in pFmt with the default specifier if supported or with %s otherwise
// writes to pOutBuffer, returns amount of bytes written
size_t RewriteFormatString(char* __restrict pOutBuffer, size_t outBufferSize, char const* __restrict pFmt, char const* const* __restrict ppDefaultSpecifiersPerArg, size_t numArgs)
{
    CC_ASSERT(pOutBuffer && outBufferSize && pFmt);
    CC_ASSUME(pOutBuffer && outBufferSize && pFmt);

    char* bufHead = pOutBuffer;

    size_t nextArgIndex = 0;
    bool bIsSpecifierStarted = false;

    constexpr char genericSpecifier = 'k';
    static_assert(!IsPrintfSpecifierPrefix(genericSpecifier), "Generic specifier could be the legitimate start of a printf specifier");

    for (char const* p = pFmt; *p != '\0'; ++p)
    {
        bool const bIsGenericSpecifier = (p[0] == '%' && p[1] == genericSpecifier) //
                                         || (p[0] == '{' && p[1] == '}');          // backward compat with old syntax

        if (bIsGenericSpecifier && !bIsSpecifierStarted)
        {
            CC_ASSERT(nextArgIndex < numArgs && "More specifiers than arguments");

            char const* defaultSpecifier = ppDefaultSpecifiersPerArg[nextArgIndex++];
            if (!defaultSpecifier)
            {
                // arg has no native printf support
                // write %s, the argument will replaced with a string by custom formatting
                defaultSpecifier = "%s";
            }

            while (*defaultSpecifier)
            {
                *(bufHead++) = *(defaultSpecifier++);
            }

            // skip the generic identifier
            ++p;
            continue;
        }

        if (bIsSpecifierStarted && *p != '%' && *p != '\0')
        {
            // CC_ASSERT(nextArgIndex < numArgs && "More specifiers than arguments");

            // this is the start of a non-generic non-escaped printf specifier
            nextArgIndex++;
            bIsSpecifierStarted = false;
        }

        *(bufHead++) = *p;

        if (p[0] == '%')
        {
            bIsSpecifierStarted = !bIsSpecifierStarted;
        }
    }

    *(bufHead++) = '\0';
    CC_ASSERT((bufHead - pOutBuffer) < outBufferSize && "Format string too long");
    return bufHead - pOutBuffer;
}
}

RLOG_API int rlog::detail::RunCustomSnprintf(
    char* __restrict pOutBuffer, size_t bufferSize, char const* const* __restrict ppDefaultSpecifiersPerArg, size_t numArgs, char const* __restrict pOriginalFmt, ...)
{
    char fmtBuf[2048];
    RewriteFormatString(fmtBuf, sizeof(fmtBuf), pOriginalFmt, ppDefaultSpecifiersPerArg, numArgs);

    char const* const pModifiedFmt = fmtBuf;

    va_list vlist;
    // NOTE: the second argument is the NAME of the function parameter right before the '...' (syntactically!)
    // It's NOT the format string to use!
    va_start(vlist, pOriginalFmt);
    int const res = ::rich_log_stbsp_vsnprintf(pOutBuffer, int(bufferSize), pModifiedFmt, vlist);
    va_end(vlist);
    return res;
}
