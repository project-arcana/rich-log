#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <type_traits>

#include <clean-core/macros.hh>

#include <rich-log/detail/api.hh>

namespace rlog::detail
{
template <class T>
constexpr char const* GetDefaultPrintfSpecifier()
{
    using DecayedType = std::decay_t<T>;

#define IFIS_TYPE(Type) if constexpr (std::is_same_v<DecayedType, Type>)
#define ELIF_TYPE(Type) else IFIS_TYPE(Type)

    IFIS_TYPE(char) return "%c";
    ELIF_TYPE(bool) return "%d";
    ELIF_TYPE(float) return "%.4f";
    ELIF_TYPE(double) return "%.6f";
    ELIF_TYPE(signed char) return "%d";
    ELIF_TYPE(signed short) return "%d";
    ELIF_TYPE(signed int) return "%d";
    ELIF_TYPE(signed long) return "%ld";
    ELIF_TYPE(signed long long) return "%lld";
    ELIF_TYPE(unsigned char) return "%u";
    ELIF_TYPE(unsigned short) return "%u";
    ELIF_TYPE(unsigned int) return "%u";
    ELIF_TYPE(unsigned long) return "%lu";
    ELIF_TYPE(unsigned long long) return "%llu";
    ELIF_TYPE(char*) return "%s";
    ELIF_TYPE(char const*) return "%s";
    ELIF_TYPE(decltype(nullptr)) return "%p";
    else if constexpr (std::is_pointer_v<DecayedType>) return "%p";
    else { return nullptr; }

#undef IFIS_TYPE
#undef ELIF_TYPE
}

template <class T>
static constexpr bool HasTypePrintfSupport = GetDefaultPrintfSpecifier<T>() != nullptr;

template <class Container>
auto HasCStrMethodTest(Container* c) -> decltype(static_cast<char const*>(c->c_str()), int(0));
template <class Container>
char HasCStrMethodTest(...);

template <class Container>
static constexpr bool HasTypeCStrMethod = sizeof(HasCStrMethodTest<std::remove_reference_t<Container>>(nullptr)) == sizeof(int);

template <class T>
CC_FORCE_INLINE auto GetStringifiedOrPassthrough(T const& v, char** ppHead, char const* pEnd)
{
    if constexpr (HasTypePrintfSupport<T>)
    {
        // passthrough
        return v;
    }
    else if constexpr (HasTypeCStrMethod<T>)
    {
        // no need to allocate/copy anything if a c string is already provided
        return v.c_str();
    }
    else
    {
        char* const pHead = *ppHead;
        size_t const length = to_string(pHead, v);
        CC_ASSERT(pHead + length < pEnd && "Formatted arguments too long");
        (*ppHead) += length;
        return pHead;
    }
}

// replaces all generic specifiers %k
RLOG_API size_t RewriteFormatString(
    char* __restrict pOutBuffer, size_t outBufferSize, char const* __restrict pFmt, char const* const* __restrict ppDefaultSpecifiersPerArg, size_t numArgs);

template <class... Args>
int CustomSnprintf(char* pOutBuffer, size_t bufferSize, char const* __restrict pFmt, Args const&... args)
{
    CC_ASSERT(pFmt && pOutBuffer && bufferSize);
    CC_ASSUME(pFmt && pOutBuffer && bufferSize);

    // contains [null, "s", null, "d", ".4f"] for args [Foo{}, char const*, Bar{}, int, float]
    constexpr char const* const arrDefaultSpecifiers[] = {GetDefaultPrintfSpecifier<Args>()...};

    // replace all generic specifiers with the default printf specifier, or with %s if it requires custom formatting
    char fmtBuf[4096];
    size_t const fmtOffset = RewriteFormatString(fmtBuf, sizeof(fmtBuf), pFmt, arrDefaultSpecifiers, CC_COUNTOF(arrDefaultSpecifiers));

    // use the remaining space in fmtBuf to allocate results of custom formatting
    char* pFormatArgsHead = fmtBuf + fmtOffset;
    char* const pFormatArgsEnd = fmtBuf + sizeof(fmtBuf);

    // call the real variadic function
    return ::snprintf(pOutBuffer, bufferSize, fmtBuf, GetStringifiedOrPassthrough(args, &pFormatArgsHead, pFormatArgsEnd)...);
}

}