#pragma once

#include <stddef.h>

#include <type_traits>

#include <clean-core/macros.hh>

#include <reflector/to_string.hh>

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

// copies up to (destSize - 1) bytes from src to dest, then 0-terminates the written string
// returns the strlen of the string it tried to create (even if dest was too small)
CC_FORCE_INLINE size_t Strlcpy(char* __restrict pDestination, char const* __restrict pSource, size_t nDestCapacity)
{
    const char* s = pSource;
    size_t n = nDestCapacity;

    if (n && --n)
    {
        do
        {
            if ((*pDestination++ = *s++) == 0)
                break;
        } while (--n);
    }

    if (!n)
    {
        if (nDestCapacity)
            *pDestination = 0;
        while (*s++)
        {
        }
    }

    return (size_t)(s - pSource - 1);
}

template <class T>
CC_FORCE_INLINE auto GetStringifiedOrPassthrough(T const& v, char** ppHead, char const* pEnd)
{
    if constexpr (HasTypePrintfSupport<T>)
    {
        // passthrough
        //
        // returning by value is alright, the type is <= 8 byte
        // sizeof(T) might still be > 8, for example for args like 'char buf[512]'
        // however this function has return type auto, not decltype(auto), meaning it decays these args to char*
        return v;
    }
    else if constexpr (HasTypeCStrMethod<T>)
    {
        // no need to allocate/copy anything if a c string is already provided
        return static_cast<char const*>(v.c_str());

        // theoretically we could translate string view types without copying as well
        // by using %.*s and splitting the arg into (int)v.size(), v.data()
        // but splitting a single template-variadic arg into two c-variadic args conditionally would complicate things a lot
    }
    else
    {
        char* const pHead = *ppHead;

        // TODO: if we had a version of this with signature
        // 'int to_string(char* pBuf, size_t size, T const& val)'
        // this could be made much more efficient, right now we potentially allocate per arg and have superfluous copies
#if 0
        size_t const length = to_string(pHead, v);
        CC_ASSERT(pHead + length < pEnd && "Formatted arguments too long");
        (*ppHead) += length;
#else
        cc::string asString = rf::to_string(v);

        // strlcpy returns strlen, add one for 0-termination
        size_t const length = Strlcpy(pHead, asString.c_str(), pEnd - pHead) + 1;

        // this could clamp to pEnd instead and warn/truncate silently if we want to
        CC_ASSERT(pHead + length < pEnd && "Formatted arguments too long");
        (*ppHead) += length;
#endif

        return pHead;
    }
}

// pOutBuffer: the buffer to write the result to
// bufferSize: size of pOutBuffer
// the default specifier for each of the arguments in order, or null if the arg type is unsupported
RLOG_API int RunCustomSnprintf(char* __restrict pOutBuffer,
                               size_t bufferSize,
                               char const* const* __restrict ppDefaultSpecifiersPerArg,
                               size_t numArgs,
                               char const* __restrict pOriginalFmt,
                               ...);

// Behaves like snprintf, but supports a generic format specifier %k that automatically handles any type, including
// stringifiable composite types
template <class... Args>
int CustomSnprintf(char* pOutBuffer, size_t bufferSize, char const* __restrict pFmt, Args&&... args)
{
    CC_ASSERT(pFmt && pOutBuffer && bufferSize);
    CC_ASSUME(pFmt && pOutBuffer && bufferSize);

    // contains [null, "s", null, "d", ".4f"] for args [Foo{}, char const*, Bar{}, int, float]
    // must start with nullptr to avoid zero-sized array
    static constexpr char const* const arrDefaultSpecifiers[] = {nullptr, GetDefaultPrintfSpecifier<Args>()...};

    char stringifcationBuf[2048];
    char* pBufHead = stringifcationBuf;
    char const* pBufEnd = stringifcationBuf + sizeof(stringifcationBuf);

    return RunCustomSnprintf(pOutBuffer, bufferSize, arrDefaultSpecifiers + 1, sizeof...(Args), pFmt, GetStringifiedOrPassthrough(args, &pBufHead, pBufEnd)...);
}

}