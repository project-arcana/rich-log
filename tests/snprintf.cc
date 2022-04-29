#include <nexus/test.hh>

#include <cstdio>

#include <rich-log/detail/CustomPrintf.hh>

TEST("Custom snprintf parity")
{
    char bufStd[1024] = {};
    char bufRlog[1024] = {};

#define TEST_VS_STDLIB(Format, ...)                                                                \
    do                                                                                             \
    {                                                                                              \
        int numRlog = rlog::detail::CustomSnprintf(bufRlog, sizeof(bufRlog), Format, __VA_ARGS__); \
        int numStd = std::snprintf(bufStd, sizeof(bufStd), Format, __VA_ARGS__);                   \
        CHECK(numRlog == numStd);                                                                  \
        CHECK(!std::strcmp(bufStd, bufRlog));                                                      \
    } while (0)

    float pi = 3.14152f;

    TEST_VS_STDLIB("");
    TEST_VS_STDLIB("Test");
    TEST_VS_STDLIB("%% Test %%");
    TEST_VS_STDLIB("Float formatting: %f", pi);
    TEST_VS_STDLIB("Float formatting: %.2f", pi);
    TEST_VS_STDLIB("Scientific float: %e", 1e10f);
    TEST_VS_STDLIB("%%%e %.1f %.5f %.7f %+.1f%%", pi, pi, pi, pi, pi);

    char const* strLiteral = "string containing %f %d %u %zu %p %%";
    TEST_VS_STDLIB("String formatting %s", strLiteral);
    TEST_VS_STDLIB("String formatting %.5s", strLiteral);
    TEST_VS_STDLIB("String formatting %c", strLiteral[0]);

    char charArray[] = "charArrayNullTerminated %f %e %% %%%";
    TEST_VS_STDLIB("String %s formatting %s from %s array %s", charArray, charArray + 1, charArray + 2, charArray + 3);
}