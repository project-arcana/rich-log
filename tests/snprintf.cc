#include <nexus/test.hh>

#include <cstdio>

#include <rich-log/detail/CustomPrintf.hh>

// mismatching snprintf argument types
#pragma warning(disable : 4477)
#pragma warning(disable : 4313)
// too many snprintf args
#pragma warning(disable : 4474)

#define ENABLE_INVALID_ARG_AMOUNT_TESTS true

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
        /*puts(bufRlog);*/                                                                         \
    } while (0)

    // some things are implementation defined (NaN, Inf, pointer representation), but they must still not crash
#define TEST_DOES_NOT_CRASH(Format, ...)                                                           \
    do                                                                                             \
    {                                                                                              \
        int numRlog = rlog::detail::CustomSnprintf(bufRlog, sizeof(bufRlog), Format, __VA_ARGS__); \
        int numStd = std::snprintf(bufStd, sizeof(bufStd), Format, __VA_ARGS__);                   \
        CHECK(numRlog > 0);                                                                        \
        CHECK(numStd > 0);                                                                         \
    } while (0)

    float pi = 3.14152f;
    char const* s2 = "%%";

    TEST_VS_STDLIB("");
    TEST_VS_STDLIB("Test");
    TEST_VS_STDLIB("%% Test %%");
    TEST_VS_STDLIB("Float formatting: %f", pi);
    TEST_VS_STDLIB("Float formatting: %.2f", pi);
    TEST_VS_STDLIB("Scientific float: %e", 1e10f);
    TEST_VS_STDLIB("%%%e %.1f %.5f %.7f %+.1f%%", pi, pi, pi, pi, pi);

    // these differ by case and NaN vs nan(ind)
    TEST_DOES_NOT_CRASH("%f %.1f %.5f %+.2f", NAN, NAN, NAN, NAN);
    TEST_DOES_NOT_CRASH("%f %.1f %.5f %+.2f", INFINITY, INFINITY, INFINITY, INFINITY);

    char const* strLiteral = "string containing %f %d %u %zu %p %%";
    TEST_VS_STDLIB("String formatting %s", strLiteral);
    TEST_VS_STDLIB("String formatting %.5s", strLiteral);
    TEST_VS_STDLIB("String formatting %c", strLiteral[0]);

    char charArray[] = "charArrayNullTerminated %f %e %% %%%";
    TEST_VS_STDLIB("String %s formatting %s from %s array %s", charArray, charArray + 1, charArray + 2, charArray + 3);

    // list from https://github.com/Tripouille/printfTester/blob/master/tests/x_test.cpp and related files:

    TEST_VS_STDLIB(" %x ", 0);
    TEST_VS_STDLIB(" %x ", -1);
    TEST_VS_STDLIB(" %x ", 1);
    TEST_VS_STDLIB(" %x ", 9);
    TEST_VS_STDLIB(" %x ", 10);
    TEST_VS_STDLIB(" %x ", 11);
    TEST_VS_STDLIB(" %x ", 15);
    TEST_VS_STDLIB(" %x ", 16);
    TEST_VS_STDLIB(" %x ", 17);
    TEST_VS_STDLIB(" %x ", 99);
    TEST_VS_STDLIB(" %x ", 100);
    TEST_VS_STDLIB(" %x ", 101);
    TEST_VS_STDLIB(" %x ", -9);
    TEST_VS_STDLIB(" %x ", -10);
    TEST_VS_STDLIB(" %x ", -11);
    TEST_VS_STDLIB(" %x ", -14);
    TEST_VS_STDLIB(" %x ", -15);
    TEST_VS_STDLIB(" %x ", -16);
    TEST_VS_STDLIB(" %x ", -99);
    TEST_VS_STDLIB(" %x ", -100);
    TEST_VS_STDLIB(" %x ", -101);
    TEST_VS_STDLIB(" %x ", INT_MAX);
    TEST_VS_STDLIB(" %x ", INT_MIN);
    TEST_VS_STDLIB(" %x ", LONG_MAX);
    TEST_VS_STDLIB(" %x ", LONG_MIN);
    TEST_VS_STDLIB(" %x ", UINT_MAX);
    TEST_VS_STDLIB(" %x ", ULONG_MAX);
    TEST_VS_STDLIB(" %x ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %x %x %x %x %x %x %x", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);
    TEST_VS_STDLIB(" %x ", 42);
    TEST_VS_STDLIB(" %x ", -42);

    TEST_VS_STDLIB(" %#x ", 0);
    TEST_VS_STDLIB(" %#x ", -1);
    TEST_VS_STDLIB(" %#x ", 1);
    TEST_VS_STDLIB(" %#x ", 9);
    TEST_VS_STDLIB(" %#x ", 10);
    TEST_VS_STDLIB(" %#x ", 11);
    TEST_VS_STDLIB(" %#x ", 15);
    TEST_VS_STDLIB(" %#x ", 16);
    TEST_VS_STDLIB(" %#x ", 17);
    TEST_VS_STDLIB(" %#x ", 99);
    TEST_VS_STDLIB(" %#x ", 100);
    TEST_VS_STDLIB(" %#x ", 101);
    TEST_VS_STDLIB(" %#x ", -9);
    TEST_VS_STDLIB(" %#x ", -10);
    TEST_VS_STDLIB(" %#x ", -11);
    TEST_VS_STDLIB(" %#x ", -14);
    TEST_VS_STDLIB(" %#x ", -15);
    TEST_VS_STDLIB(" %#x ", -16);
    TEST_VS_STDLIB(" %#x ", -99);
    TEST_VS_STDLIB(" %#x ", -100);
    TEST_VS_STDLIB(" %#x ", -101);
    TEST_VS_STDLIB(" %#x ", INT_MAX);
    TEST_VS_STDLIB(" %#x ", INT_MIN);
    TEST_VS_STDLIB(" %#x ", LONG_MAX);
    TEST_VS_STDLIB(" %#x ", LONG_MIN);
    TEST_VS_STDLIB(" %#x ", UINT_MAX);
    TEST_VS_STDLIB(" %#x ", ULONG_MAX);
    TEST_VS_STDLIB(" %#x ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %#x %#x %#x %#x %#x %#x %#x", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);
    TEST_VS_STDLIB(" %#x%#x ", INT_MAX, INT_MIN);

    TEST_VS_STDLIB(" %#X ", 0);
    TEST_VS_STDLIB(" %#X ", -1);
    TEST_VS_STDLIB(" %#X ", 1);
    TEST_VS_STDLIB(" %#X ", 9);
    TEST_VS_STDLIB(" %#X ", 10);
    TEST_VS_STDLIB(" %#X ", 11);
    TEST_VS_STDLIB(" %#X ", 15);
    TEST_VS_STDLIB(" %#X ", 16);
    TEST_VS_STDLIB(" %#X ", 17);
    TEST_VS_STDLIB(" %#X ", 99);
    TEST_VS_STDLIB(" %#X ", 100);
    TEST_VS_STDLIB(" %#X ", 101);
    TEST_VS_STDLIB(" %#X ", -9);
    TEST_VS_STDLIB(" %#X ", -10);
    TEST_VS_STDLIB(" %#X ", -11);
    TEST_VS_STDLIB(" %#X ", -14);
    TEST_VS_STDLIB(" %#X ", -15);
    TEST_VS_STDLIB(" %#X ", -16);
    TEST_VS_STDLIB(" %#X ", -99);
    TEST_VS_STDLIB(" %#X ", -100);
    TEST_VS_STDLIB(" %#X ", -101);
    TEST_VS_STDLIB(" %#X ", INT_MAX);
    TEST_VS_STDLIB(" %#X ", INT_MIN);
    TEST_VS_STDLIB(" %#X ", LONG_MAX);
    TEST_VS_STDLIB(" %#X ", LONG_MIN);
    TEST_VS_STDLIB(" %#X ", UINT_MAX);
    TEST_VS_STDLIB(" %#X ", ULONG_MAX);
    TEST_VS_STDLIB(" %#X ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %#X %#X %#X %#X %#X %#X %#X", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %u ", 0);
    TEST_VS_STDLIB(" %u ", -1);
    TEST_VS_STDLIB(" %u ", 1);
    TEST_VS_STDLIB(" %u ", 9);
    TEST_VS_STDLIB(" %u ", 10);
    TEST_VS_STDLIB(" %u ", 11);
    TEST_VS_STDLIB(" %u ", 15);
    TEST_VS_STDLIB(" %u ", 16);
    TEST_VS_STDLIB(" %u ", 17);
    TEST_VS_STDLIB(" %u ", 99);
    TEST_VS_STDLIB(" %u ", 100);
    TEST_VS_STDLIB(" %u ", 101);
    TEST_VS_STDLIB(" %u ", -9);
    TEST_VS_STDLIB(" %u ", -10);
    TEST_VS_STDLIB(" %u ", -11);
    TEST_VS_STDLIB(" %u ", -14);
    TEST_VS_STDLIB(" %u ", -15);
    TEST_VS_STDLIB(" %u ", -16);
    TEST_VS_STDLIB(" %u ", -99);
    TEST_VS_STDLIB(" %u ", -100);
    TEST_VS_STDLIB(" %u ", -101);
    TEST_VS_STDLIB(" %u ", INT_MAX);
    TEST_VS_STDLIB(" %u ", INT_MIN);
    TEST_VS_STDLIB(" %u ", LONG_MAX);
    TEST_VS_STDLIB(" %u ", LONG_MIN);
    TEST_VS_STDLIB(" %u ", UINT_MAX);
    TEST_VS_STDLIB(" %u ", ULONG_MAX);
    TEST_VS_STDLIB(" %u ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %u %u %u %u %u %u %u", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB("% s", "");
    TEST_VS_STDLIB(" % 1s", "");
    TEST_VS_STDLIB("% 1s ", "");
    TEST_VS_STDLIB(" % s ", "");
    TEST_VS_STDLIB(" % s ", "-");
    TEST_VS_STDLIB(" % s % s ", "", "-");
    TEST_VS_STDLIB(" % s % s ", " - ", "");
    TEST_VS_STDLIB(" % s % s % s % s ", " - ", "", "4", "");
    TEST_VS_STDLIB(" % s % s % s % s % s ", " - ", "", "4", "", "2 ");

    TEST_VS_STDLIB(" % d ", 0);
    TEST_VS_STDLIB(" % d ", -1);
    TEST_VS_STDLIB(" % d ", 1);
    TEST_VS_STDLIB(" % d ", 9);
    TEST_VS_STDLIB(" % d ", 10);
    TEST_VS_STDLIB(" % d ", 11);
    TEST_VS_STDLIB(" % d ", 15);
    TEST_VS_STDLIB(" % d ", 16);
    TEST_VS_STDLIB(" % d ", 17);
    TEST_VS_STDLIB(" % d ", 99);
    TEST_VS_STDLIB(" % d ", 100);
    TEST_VS_STDLIB(" % d ", 101);
    TEST_VS_STDLIB(" % d ", -9);
    TEST_VS_STDLIB(" % d ", -10);
    TEST_VS_STDLIB(" % d ", -11);
    TEST_VS_STDLIB(" % d ", -14);
    TEST_VS_STDLIB(" % d ", -15);
    TEST_VS_STDLIB(" % d ", -16);
    TEST_VS_STDLIB(" % d ", -99);
    TEST_VS_STDLIB(" % d ", -100);
    TEST_VS_STDLIB(" % d ", -101);
    TEST_VS_STDLIB(" % d ", INT_MAX);
    TEST_VS_STDLIB(" % d ", INT_MIN);
    TEST_VS_STDLIB(" % d ", LONG_MAX);
    TEST_VS_STDLIB(" % d ", LONG_MIN);
    TEST_VS_STDLIB(" % d ", UINT_MAX);
    TEST_VS_STDLIB(" % d ", ULONG_MAX);
    TEST_VS_STDLIB(" % d ", 9223372036854775807LL);
    TEST_VS_STDLIB(" % d % d % d % d % d % d % d", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" % i ", 0);
    TEST_VS_STDLIB(" % i ", -1);
    TEST_VS_STDLIB(" % i ", 1);
    TEST_VS_STDLIB(" % i ", 9);
    TEST_VS_STDLIB(" % i ", 10);
    TEST_VS_STDLIB(" % i ", 11);
    TEST_VS_STDLIB(" % i ", 15);
    TEST_VS_STDLIB(" % i ", 16);
    TEST_VS_STDLIB(" % i ", 17);
    TEST_VS_STDLIB(" % i ", 99);
    TEST_VS_STDLIB(" % i ", 100);
    TEST_VS_STDLIB(" % i ", 101);
    TEST_VS_STDLIB(" % i ", -9);
    TEST_VS_STDLIB(" % i ", -10);
    TEST_VS_STDLIB(" % i ", -11);
    TEST_VS_STDLIB(" % i ", -14);
    TEST_VS_STDLIB(" % i ", -15);
    TEST_VS_STDLIB(" % i ", -16);
    TEST_VS_STDLIB(" % i ", -99);
    TEST_VS_STDLIB(" % i ", -100);
    TEST_VS_STDLIB(" % i ", -101);
    TEST_VS_STDLIB(" % i ", INT_MAX);
    TEST_VS_STDLIB(" % i ", INT_MIN);
    TEST_VS_STDLIB(" % i ", LONG_MAX);
    TEST_VS_STDLIB(" % i ", LONG_MIN);
    TEST_VS_STDLIB(" % i ", UINT_MAX);
    TEST_VS_STDLIB(" % i ", ULONG_MAX);
    TEST_VS_STDLIB(" % i ", 9223372036854775807LL);
    TEST_VS_STDLIB(" % i % i % i % i % i % i % i", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB("%s", "");
    TEST_VS_STDLIB(" %s", "");
    TEST_VS_STDLIB("%s ", "");
    TEST_VS_STDLIB(" %s ", "");
    TEST_VS_STDLIB(" %s ", "-");
    TEST_VS_STDLIB(" %s %s ", "", "-");
    TEST_VS_STDLIB(" %s %s ", " - ", "");
    TEST_VS_STDLIB(" %s %s %s %s %s", " - ", "", "4", "", s2);
    TEST_VS_STDLIB(" %s %s %s %s %s ", " - ", "", "4", "", "2 ");

    TEST_DOES_NOT_CRASH(" NULL %s NULL ", nullptr);

    TEST_VS_STDLIB(" %% ");
    TEST_VS_STDLIB(" %%%% ");
    TEST_VS_STDLIB(" %% %% %% ");
    TEST_VS_STDLIB(" %%  %%  %% ");
    TEST_VS_STDLIB(" %%   %%   %% ");
    TEST_VS_STDLIB("%%");
    TEST_VS_STDLIB("%% %%");

    TEST_DOES_NOT_CRASH(" %p ", -1);
    TEST_DOES_NOT_CRASH(" %p ", 1);
    TEST_DOES_NOT_CRASH(" %p ", 15);
    TEST_DOES_NOT_CRASH(" %p ", 16);
    TEST_DOES_NOT_CRASH(" %p ", 17);
    TEST_DOES_NOT_CRASH(" %p %p ", LONG_MIN, LONG_MAX);
    TEST_DOES_NOT_CRASH(" %p %p ", INT_MIN, INT_MAX);
    TEST_DOES_NOT_CRASH(" %p %p ", ULONG_MAX, -ULONG_MAX);
    TEST_DOES_NOT_CRASH(" %p %p ", 0, 0);

    TEST_VS_STDLIB("%%%c%%%s%%%d%%%i%%%u%%%x%%%X%%%% %%%c%%%s%%%d%%%i%%%u%%%x%%%X%%%% %%%c%%%s%%%d%%%i%%%u%%%x%%%X%%%% %c%%", 'A', "42", 42, 42, 42,
                   42, 42, 'B', "-42", -42, -42, -42, -42, 42, 'C', "0", 0, 0, 0, 0, 42, 0);

    TEST_VS_STDLIB("%-1c", '0');
    TEST_VS_STDLIB(" %-2c ", '0');
    TEST_VS_STDLIB(" %-3c", '0' - 256);
    TEST_VS_STDLIB("%-4c ", '0' + 256);
    TEST_VS_STDLIB(" %-1c %-2c %-3c ", '0', 0, '1');
    TEST_VS_STDLIB(" %-1c %-2c %-3c ", ' ', ' ', ' ');
    TEST_VS_STDLIB(" %-1c %-2c %-3c ", '1', '2', '3');
    TEST_VS_STDLIB(" %-1c %-2c %-3c ", '2', '1', 0);
    TEST_VS_STDLIB(" %-1c %-2c %-3c ", 0, '1', '2');

    TEST_VS_STDLIB("%1s", "");
    TEST_VS_STDLIB("%-1s", "");
    TEST_VS_STDLIB(" %-2s", "");
    TEST_VS_STDLIB("%-3s ", "");
    TEST_VS_STDLIB(" %-0s ", "");
    TEST_VS_STDLIB(" %-s ", "-");
    TEST_VS_STDLIB(" %-1s %-2s ", "", "-");
    TEST_VS_STDLIB(" %-3s %-4s ", " - ", "");
    TEST_VS_STDLIB(" %-2s %-3s %-4s %-5s ", " - ", "", "4", "");
    TEST_VS_STDLIB(" %-3s %-3s %-4s %-5s %-3s ", " - ", "", "4", "", "2 ");

    TEST_DOES_NOT_CRASH(" %2p ", -1);
    TEST_DOES_NOT_CRASH(" %-2p ", -1);
    TEST_DOES_NOT_CRASH(" %-2p ", 1);
    TEST_DOES_NOT_CRASH(" %-2p ", 15);
    TEST_DOES_NOT_CRASH(" %-3p ", 16);
    TEST_DOES_NOT_CRASH(" %-4p ", 17);
    TEST_DOES_NOT_CRASH(" %-9p %-10p ", LONG_MIN, LONG_MAX);
    TEST_DOES_NOT_CRASH(" %-11p %-12p ", INT_MIN, INT_MAX);
    TEST_DOES_NOT_CRASH(" %-13p %-14p ", ULONG_MAX, -ULONG_MAX);
    TEST_DOES_NOT_CRASH(" %-1p %-2p ", 0, 0);

    TEST_VS_STDLIB(" %-1d ", 0);
    TEST_VS_STDLIB(" %-2d ", -1);
    TEST_VS_STDLIB(" %-3d ", 1);
    TEST_VS_STDLIB(" %-4d ", 9);
    TEST_VS_STDLIB(" %-1d ", 10);
    TEST_VS_STDLIB(" %-2d ", 11);
    TEST_VS_STDLIB(" %-3d ", 15);
    TEST_VS_STDLIB(" %-4d ", 16);
    TEST_VS_STDLIB(" %-5d ", 17);
    TEST_VS_STDLIB(" %-1d ", 99);
    TEST_VS_STDLIB(" %-2d ", 100);
    TEST_VS_STDLIB(" %-3d ", 101);
    TEST_VS_STDLIB(" %-1d ", -9);
    TEST_VS_STDLIB(" %-2d ", -10);
    TEST_VS_STDLIB(" %-3d ", -11);
    TEST_VS_STDLIB(" %-4d ", -14);
    TEST_VS_STDLIB(" %-5d ", -15);
    TEST_VS_STDLIB(" %-6d ", -16);
    TEST_VS_STDLIB(" %-1d ", -99);
    TEST_VS_STDLIB(" %-2d ", -100);
    TEST_VS_STDLIB(" %-3d ", -101);
    TEST_VS_STDLIB(" %-9d ", INT_MAX);
    TEST_VS_STDLIB(" %-10d ", INT_MIN);
    TEST_VS_STDLIB(" %-11d ", LONG_MAX);
    TEST_VS_STDLIB(" %-12d ", LONG_MIN);
    TEST_VS_STDLIB(" %-13d ", UINT_MAX);
    TEST_VS_STDLIB(" %-14d ", ULONG_MAX);
    TEST_VS_STDLIB(" %-15d ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %-9d %-10d %-11d %-12d %-13d %-14d %-15d", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %-1i ", 0);
    TEST_VS_STDLIB(" %-2i ", -1);
    TEST_VS_STDLIB(" %-3i ", 1);
    TEST_VS_STDLIB(" %-4i ", 9);
    TEST_VS_STDLIB(" %-1i ", 10);
    TEST_VS_STDLIB(" %-2i ", 11);
    TEST_VS_STDLIB(" %-3i ", 15);
    TEST_VS_STDLIB(" %-4i ", 16);
    TEST_VS_STDLIB(" %-5i ", 17);
    TEST_VS_STDLIB(" %-1i ", 99);
    TEST_VS_STDLIB(" %-2i ", 100);
    TEST_VS_STDLIB(" %-3i ", 101);
    TEST_VS_STDLIB(" %-1i ", -9);
    TEST_VS_STDLIB(" %-2i ", -10);
    TEST_VS_STDLIB(" %-3i ", -11);
    TEST_VS_STDLIB(" %-4i ", -14);
    TEST_VS_STDLIB(" %-5i ", -15);
    TEST_VS_STDLIB(" %-6i ", -16);
    TEST_VS_STDLIB(" %-1i ", -99);
    TEST_VS_STDLIB(" %-2i ", -100);
    TEST_VS_STDLIB(" %-3i ", -101);
    TEST_VS_STDLIB(" %-9i ", INT_MAX);
    TEST_VS_STDLIB(" %-10i ", INT_MIN);
    TEST_VS_STDLIB(" %-11i ", LONG_MAX);
    TEST_VS_STDLIB(" %-12i ", LONG_MIN);
    TEST_VS_STDLIB(" %-13i ", UINT_MAX);
    TEST_VS_STDLIB(" %-14i ", ULONG_MAX);
    TEST_VS_STDLIB(" %-15i ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %-9i %-10i %-11i %-12i %-13i %-14i %-15i", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %-1u ", 0);
    TEST_VS_STDLIB(" %-2u ", -1);
    TEST_VS_STDLIB(" %-3u ", 1);
    TEST_VS_STDLIB(" %-4u ", 9);
    TEST_VS_STDLIB(" %-1u ", 10);
    TEST_VS_STDLIB(" %-2u ", 11);
    TEST_VS_STDLIB(" %-3u ", 15);
    TEST_VS_STDLIB(" %-4u ", 16);
    TEST_VS_STDLIB(" %-5u ", 17);
    TEST_VS_STDLIB(" %-1u ", 99);
    TEST_VS_STDLIB(" %-2u ", 100);
    TEST_VS_STDLIB(" %-3u ", 101);
    TEST_VS_STDLIB(" %-1u ", -9);
    TEST_VS_STDLIB(" %-2u ", -10);
    TEST_VS_STDLIB(" %-3u ", -11);
    TEST_VS_STDLIB(" %-4u ", -14);
    TEST_VS_STDLIB(" %-5u ", -15);
    TEST_VS_STDLIB(" %-6u ", -16);
    TEST_VS_STDLIB(" %-1u ", -99);
    TEST_VS_STDLIB(" %-2u ", -100);
    TEST_VS_STDLIB(" %-3u ", -101);
    TEST_VS_STDLIB(" %-9u ", INT_MAX);
    TEST_VS_STDLIB(" %-10u ", INT_MIN);
    TEST_VS_STDLIB(" %-11u ", LONG_MAX);
    TEST_VS_STDLIB(" %-12u ", LONG_MIN);
    TEST_VS_STDLIB(" %-13u ", UINT_MAX);
    TEST_VS_STDLIB(" %-14u ", ULONG_MAX);
    TEST_VS_STDLIB(" %-15u ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %-9u %-10u %-11u %-12u %-13u %-14u %-15u", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %-1x ", 0);
    TEST_VS_STDLIB(" %-2x ", -1);
    TEST_VS_STDLIB(" %-3x ", 1);
    TEST_VS_STDLIB(" %-4x ", 9);
    TEST_VS_STDLIB(" %-1x ", 10);
    TEST_VS_STDLIB(" %-2x ", 11);
    TEST_VS_STDLIB(" %-3x ", 15);
    TEST_VS_STDLIB(" %-4x ", 16);
    TEST_VS_STDLIB(" %-5x ", 17);
    TEST_VS_STDLIB(" %-1x ", 99);
    TEST_VS_STDLIB(" %-2x ", 100);
    TEST_VS_STDLIB(" %-3x ", 101);
    TEST_VS_STDLIB(" %-1x ", -9);
    TEST_VS_STDLIB(" %-2x ", -10);
    TEST_VS_STDLIB(" %-3x ", -11);
    TEST_VS_STDLIB(" %-4x ", -14);
    TEST_VS_STDLIB(" %-5x ", -15);
    TEST_VS_STDLIB(" %-6x ", -16);
    TEST_VS_STDLIB(" %-1x ", -99);
    TEST_VS_STDLIB(" %-2x ", -100);
    TEST_VS_STDLIB(" %-3x ", -101);
    TEST_VS_STDLIB(" %-9x ", INT_MAX);
    TEST_VS_STDLIB(" %-10x ", INT_MIN);
    TEST_VS_STDLIB(" %-11x ", LONG_MAX);
    TEST_VS_STDLIB(" %-12x ", LONG_MIN);
    TEST_VS_STDLIB(" %-13x ", UINT_MAX);
    TEST_VS_STDLIB(" %-14x ", ULONG_MAX);
    TEST_VS_STDLIB(" %-15x ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %-9x %-10x %-11x %-12x %-13x %-14x %-15x", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %-1X ", 0);
    TEST_VS_STDLIB(" %-2X ", -1);
    TEST_VS_STDLIB(" %-3X ", 1);
    TEST_VS_STDLIB(" %-4X ", 9);
    TEST_VS_STDLIB(" %-1X ", 10);
    TEST_VS_STDLIB(" %-2X ", 11);
    TEST_VS_STDLIB(" %-3X ", 15);
    TEST_VS_STDLIB(" %-4X ", 16);
    TEST_VS_STDLIB(" %-5X ", 17);
    TEST_VS_STDLIB(" %-1X ", 99);
    TEST_VS_STDLIB(" %-2X ", 100);
    TEST_VS_STDLIB(" %-3X ", 101);
    TEST_VS_STDLIB(" %-1X ", -9);
    TEST_VS_STDLIB(" %-2X ", -10);
    TEST_VS_STDLIB(" %-3X ", -11);
    TEST_VS_STDLIB(" %-4X ", -14);
    TEST_VS_STDLIB(" %-5X ", -15);
    TEST_VS_STDLIB(" %-6X ", -16);
    TEST_VS_STDLIB(" %-1X ", -99);
    TEST_VS_STDLIB(" %-2X ", -100);
    TEST_VS_STDLIB(" %-3X ", -101);
    TEST_VS_STDLIB(" %-9X ", INT_MAX);
    TEST_VS_STDLIB(" %-10X ", INT_MIN);
    TEST_VS_STDLIB(" %-11X ", LONG_MAX);
    TEST_VS_STDLIB(" %-12X ", LONG_MIN);
    TEST_VS_STDLIB(" %-13X ", UINT_MAX);
    TEST_VS_STDLIB(" %-14X ", ULONG_MAX);
    TEST_VS_STDLIB(" %-15X ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %-9X %-10X %-11X %-12X %-13X %-14X %-15X", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %i ", 0);
    TEST_VS_STDLIB(" %i ", -1);
    TEST_VS_STDLIB(" %i ", 1);
    TEST_VS_STDLIB(" %i ", 9);
    TEST_VS_STDLIB(" %i ", 10);
    TEST_VS_STDLIB(" %i ", 11);
    TEST_VS_STDLIB(" %i ", 15);
    TEST_VS_STDLIB(" %i ", 16);
    TEST_VS_STDLIB(" %i ", 17);
    TEST_VS_STDLIB(" %i ", 99);
    TEST_VS_STDLIB(" %i ", 100);
    TEST_VS_STDLIB(" %i ", 101);
    TEST_VS_STDLIB(" %i ", -9);
    TEST_VS_STDLIB(" %i ", -10);
    TEST_VS_STDLIB(" %i ", -11);
    TEST_VS_STDLIB(" %i ", -14);
    TEST_VS_STDLIB(" %i ", -15);
    TEST_VS_STDLIB(" %i ", -16);
    TEST_VS_STDLIB(" %i ", -99);
    TEST_VS_STDLIB(" %i ", -100);
    TEST_VS_STDLIB(" %i ", -101);
    TEST_VS_STDLIB(" %i ", INT_MAX);
    TEST_VS_STDLIB(" %i ", INT_MIN);
    TEST_VS_STDLIB(" %i ", LONG_MAX);
    TEST_VS_STDLIB(" %i ", LONG_MIN);
    TEST_VS_STDLIB(" %i ", UINT_MAX);
    TEST_VS_STDLIB(" %i ", ULONG_MAX);
    TEST_VS_STDLIB(" %i ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %i %i %i %i %i %i %i", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB("%.s", "");
    TEST_VS_STDLIB(" %.1s", "");
    TEST_VS_STDLIB("%.1s ", "");
    TEST_VS_STDLIB(" %.s ", "");
    TEST_VS_STDLIB(" %.s ", "-");
    TEST_VS_STDLIB(" %.2s %.1s ", "", "-");
    TEST_VS_STDLIB(" %.3s %.2s ", " - ", "");
    TEST_VS_STDLIB(" %.1s %.2s %.3s %.4s ", " - ", "", "4", "");
    TEST_VS_STDLIB(" %.2s %.3s %.4s %.5s %.1s ", " - ", "", "4", "", "2 ");

    TEST_VS_STDLIB(" %.1d ", 0);
    TEST_VS_STDLIB(" %.2d ", -1);
    TEST_VS_STDLIB(" %.2d ", 1);
    TEST_VS_STDLIB(" %.1d ", 9);
    TEST_VS_STDLIB(" %.2d ", 10);
    TEST_VS_STDLIB(" %.3d ", 11);
    TEST_VS_STDLIB(" %.4d ", 15);
    TEST_VS_STDLIB(" %.5d ", 16);
    TEST_VS_STDLIB(" %.6d ", 17);
    TEST_VS_STDLIB(" %.1d ", 99);
    TEST_VS_STDLIB(" %.2d ", 100);
    TEST_VS_STDLIB(" %.3d ", 101);
    TEST_VS_STDLIB(" %.1d ", -9);
    TEST_VS_STDLIB(" %.2d ", -10);
    TEST_VS_STDLIB(" %.3d ", -11);
    TEST_VS_STDLIB(" %.4d ", -14);
    TEST_VS_STDLIB(" %.1d ", -15);
    TEST_VS_STDLIB(" %.2d ", -16);
    TEST_VS_STDLIB(" %.3d ", -99);
    TEST_VS_STDLIB(" %.3d ", -100);
    TEST_VS_STDLIB(" %.4d ", -101);
    TEST_VS_STDLIB(" %.8d ", INT_MAX);
    TEST_VS_STDLIB(" %.9d ", INT_MIN);
    TEST_VS_STDLIB(" %.10d ", LONG_MAX);
    TEST_VS_STDLIB(" %.11d ", LONG_MIN);
    TEST_VS_STDLIB(" %.12d ", UINT_MAX);
    TEST_VS_STDLIB(" %.13d ", ULONG_MAX);
    TEST_VS_STDLIB(" %.14d ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %.8d %.9d %.10d %.11d %.12d %.13d %.14d", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %.1i ", 0);
    TEST_VS_STDLIB(" %.2i ", -1);
    TEST_VS_STDLIB(" %.2i ", 1);
    TEST_VS_STDLIB(" %.1i ", 9);
    TEST_VS_STDLIB(" %.2i ", 10);
    TEST_VS_STDLIB(" %.3i ", 11);
    TEST_VS_STDLIB(" %.4i ", 15);
    TEST_VS_STDLIB(" %.5i ", 16);
    TEST_VS_STDLIB(" %.6i ", 17);
    TEST_VS_STDLIB(" %.1i ", 99);
    TEST_VS_STDLIB(" %.2i ", 100);
    TEST_VS_STDLIB(" %.3i ", 101);
    TEST_VS_STDLIB(" %.1i ", -9);
    TEST_VS_STDLIB(" %.2i ", -10);
    TEST_VS_STDLIB(" %.3i ", -11);
    TEST_VS_STDLIB(" %.4i ", -14);
    TEST_VS_STDLIB(" %.1i ", -15);
    TEST_VS_STDLIB(" %.2i ", -16);
    TEST_VS_STDLIB(" %.3i ", -99);
    TEST_VS_STDLIB(" %.3i ", -100);
    TEST_VS_STDLIB(" %.4i ", -101);
    TEST_VS_STDLIB(" %.8i ", INT_MAX);
    TEST_VS_STDLIB(" %.9i ", INT_MIN);
    TEST_VS_STDLIB(" %.10i ", LONG_MAX);
    TEST_VS_STDLIB(" %.11i ", LONG_MIN);
    TEST_VS_STDLIB(" %.12i ", UINT_MAX);
    TEST_VS_STDLIB(" %.13i ", ULONG_MAX);
    TEST_VS_STDLIB(" %.14i ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %.8i %.9i %.10i %.11i %.12i %.13i %.14i", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %.1u ", 0);
    TEST_VS_STDLIB(" %.2u ", -1);
    TEST_VS_STDLIB(" %.2u ", 1);
    TEST_VS_STDLIB(" %.1u ", 9);
    TEST_VS_STDLIB(" %.2u ", 10);
    TEST_VS_STDLIB(" %.3u ", 11);
    TEST_VS_STDLIB(" %.4u ", 15);
    TEST_VS_STDLIB(" %.5u ", 16);
    TEST_VS_STDLIB(" %.6u ", 17);
    TEST_VS_STDLIB(" %.1u ", 99);
    TEST_VS_STDLIB(" %.2u ", 100);
    TEST_VS_STDLIB(" %.3u ", 101);
    TEST_VS_STDLIB(" %.1u ", -9);
    TEST_VS_STDLIB(" %.2u ", -10);
    TEST_VS_STDLIB(" %.3u ", -11);
    TEST_VS_STDLIB(" %.4u ", -14);
    TEST_VS_STDLIB(" %.1u ", -15);
    TEST_VS_STDLIB(" %.2u ", -16);
    TEST_VS_STDLIB(" %.3u ", -99);
    TEST_VS_STDLIB(" %.3u ", -100);
    TEST_VS_STDLIB(" %.4u ", -101);
    TEST_VS_STDLIB(" %.8u ", INT_MAX);
    TEST_VS_STDLIB(" %.9u ", INT_MIN);
    TEST_VS_STDLIB(" %.10u ", LONG_MAX);
    TEST_VS_STDLIB(" %.11u ", LONG_MIN);
    TEST_VS_STDLIB(" %.12u ", UINT_MAX);
    TEST_VS_STDLIB(" %.13u ", ULONG_MAX);
    TEST_VS_STDLIB(" %.14u ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %.8u %.9u %.10u %.11u %.12u %.13u %.14u", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %.1x ", 0);
    TEST_VS_STDLIB(" %.2x ", -1);
    TEST_VS_STDLIB(" %.2x ", 1);
    TEST_VS_STDLIB(" %.1x ", 9);
    TEST_VS_STDLIB(" %.2x ", 10);
    TEST_VS_STDLIB(" %.3x ", 11);
    TEST_VS_STDLIB(" %.4x ", 15);
    TEST_VS_STDLIB(" %.5x ", 16);
    TEST_VS_STDLIB(" %.6x ", 17);
    TEST_VS_STDLIB(" %.1x ", 99);
    TEST_VS_STDLIB(" %.2x ", 100);
    TEST_VS_STDLIB(" %.3x ", 101);
    TEST_VS_STDLIB(" %.1x ", -9);
    TEST_VS_STDLIB(" %.2x ", -10);
    TEST_VS_STDLIB(" %.3x ", -11);
    TEST_VS_STDLIB(" %.4x ", -14);
    TEST_VS_STDLIB(" %.1x ", -15);
    TEST_VS_STDLIB(" %.2x ", -16);
    TEST_VS_STDLIB(" %.3x ", -99);
    TEST_VS_STDLIB(" %.3x ", -100);
    TEST_VS_STDLIB(" %.4x ", -101);
    TEST_VS_STDLIB(" %.8x ", INT_MAX);
    TEST_VS_STDLIB(" %.9x ", INT_MIN);
    TEST_VS_STDLIB(" %.10x ", LONG_MAX);
    TEST_VS_STDLIB(" %.11x ", LONG_MIN);
    TEST_VS_STDLIB(" %.12x ", UINT_MAX);
    TEST_VS_STDLIB(" %.13x ", ULONG_MAX);
    TEST_VS_STDLIB(" %.14x ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %.8x %.9x %.10x %.11x %.12x %.13x %.14x", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %.1X ", 0);
    TEST_VS_STDLIB(" %.2X ", -1);
    TEST_VS_STDLIB(" %.2X ", 1);
    TEST_VS_STDLIB(" %.1X ", 9);
    TEST_VS_STDLIB(" %.2X ", 10);
    TEST_VS_STDLIB(" %.3X ", 11);
    TEST_VS_STDLIB(" %.4X ", 15);
    TEST_VS_STDLIB(" %.5X ", 16);
    TEST_VS_STDLIB(" %.6X ", 17);
    TEST_VS_STDLIB(" %.1X ", 99);
    TEST_VS_STDLIB(" %.2X ", 100);
    TEST_VS_STDLIB(" %.3X ", 101);
    TEST_VS_STDLIB(" %.1X ", -9);
    TEST_VS_STDLIB(" %.2X ", -10);
    TEST_VS_STDLIB(" %.3X ", -11);
    TEST_VS_STDLIB(" %.4X ", -14);
    TEST_VS_STDLIB(" %.1X ", -15);
    TEST_VS_STDLIB(" %.2X ", -16);
    TEST_VS_STDLIB(" %.3X ", -99);
    TEST_VS_STDLIB(" %.3X ", -100);
    TEST_VS_STDLIB(" %.4X ", -101);
    TEST_VS_STDLIB(" %.8X ", INT_MAX);
    TEST_VS_STDLIB(" %.9X ", INT_MIN);
    TEST_VS_STDLIB(" %.10X ", LONG_MAX);
    TEST_VS_STDLIB(" %.11X ", LONG_MIN);
    TEST_VS_STDLIB(" %.12X ", UINT_MAX);
    TEST_VS_STDLIB(" %.13X ", ULONG_MAX);
    TEST_VS_STDLIB(" %.14X ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %.8X %.9X %.10X %.11X %.12X %.13X %.14X", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %d ", 0);
    TEST_VS_STDLIB(" %d ", -1);
    TEST_VS_STDLIB(" %d ", 1);
    TEST_VS_STDLIB(" %d ", 9);
    TEST_VS_STDLIB(" %d ", 10);
    TEST_VS_STDLIB(" %d ", 11);
    TEST_VS_STDLIB(" %d ", 15);
    TEST_VS_STDLIB(" %d ", 16);
    TEST_VS_STDLIB(" %d ", 17);
    TEST_VS_STDLIB(" %d ", 99);
    TEST_VS_STDLIB(" %d ", 100);
    TEST_VS_STDLIB(" %d ", 101);
    TEST_VS_STDLIB(" %d ", -9);
    TEST_VS_STDLIB(" %d ", -10);
    TEST_VS_STDLIB(" %d ", -11);
    TEST_VS_STDLIB(" %d ", -14);
    TEST_VS_STDLIB(" %d ", -15);
    TEST_VS_STDLIB(" %d ", -16);
    TEST_VS_STDLIB(" %d ", -99);
    TEST_VS_STDLIB(" %d ", -100);
    TEST_VS_STDLIB(" %d ", -101);
    TEST_VS_STDLIB(" %d ", INT_MAX);
    TEST_VS_STDLIB(" %d ", INT_MIN);
    TEST_VS_STDLIB(" %d ", LONG_MAX);
    TEST_VS_STDLIB(" %d ", LONG_MIN);
    TEST_VS_STDLIB(" %d ", UINT_MAX);
    TEST_VS_STDLIB(" %d ", ULONG_MAX);
    TEST_VS_STDLIB(" %d ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %d %d %d %d %d %d %d", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB("%c", '0');
    TEST_VS_STDLIB(" %c ", '0');
    TEST_VS_STDLIB(" %c", '0' - 256);
    TEST_VS_STDLIB("%c ", '0' + 256);
    TEST_VS_STDLIB(" %c %c %c ", '0', 0, '1');
    TEST_VS_STDLIB(" %c %c %c ", ' ', ' ', ' ');
    TEST_VS_STDLIB(" %c %c %c ", '1', '2', '3');
    TEST_VS_STDLIB(" %c %c %c ", '2', '1', 0);
    TEST_VS_STDLIB(" %c %c %c ", 0, '1', '2');

    TEST_VS_STDLIB(" %01d ", 0);
    TEST_VS_STDLIB(" %02d ", -1);
    TEST_VS_STDLIB(" %04d ", 9);
    TEST_VS_STDLIB(" %01d ", 10);
    TEST_VS_STDLIB(" %02d ", 11);
    TEST_VS_STDLIB(" %03d ", 15);
    TEST_VS_STDLIB(" %04d ", 16);
    TEST_VS_STDLIB(" %05d ", 17);
    TEST_VS_STDLIB(" %01d ", 99);
    TEST_VS_STDLIB(" %02d ", 100);
    TEST_VS_STDLIB(" %03d ", 101);
    TEST_VS_STDLIB(" %01d ", -9);
    TEST_VS_STDLIB(" %02d ", -10);
    TEST_VS_STDLIB(" %03d ", -11);
    TEST_VS_STDLIB(" %04d ", -14);
    TEST_VS_STDLIB(" %05d ", -15);
    TEST_VS_STDLIB(" %06d ", -16);
    TEST_VS_STDLIB(" %01d ", -99);
    TEST_VS_STDLIB(" %02d ", -100);
    TEST_VS_STDLIB(" %03d ", -101);
    TEST_VS_STDLIB(" %09d ", INT_MAX);
    TEST_VS_STDLIB(" %010d ", INT_MIN);
    TEST_VS_STDLIB(" %011d ", LONG_MAX);
    TEST_VS_STDLIB(" %012d ", LONG_MIN);
    TEST_VS_STDLIB(" %013d ", UINT_MAX);
    TEST_VS_STDLIB(" %014d ", ULONG_MAX);
    TEST_VS_STDLIB(" %015d ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %09d %010d %011d %012d %013d %014d %015d", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %01i ", 0);
    TEST_VS_STDLIB(" %02i ", -1);
    TEST_VS_STDLIB(" %03i ", 1);
    TEST_VS_STDLIB(" %04i ", 9);
    TEST_VS_STDLIB(" %01i ", 10);
    TEST_VS_STDLIB(" %02i ", 11);
    TEST_VS_STDLIB(" %03i ", 15);
    TEST_VS_STDLIB(" %04i ", 16);
    TEST_VS_STDLIB(" %05i ", 17);
    TEST_VS_STDLIB(" %01i ", 99);
    TEST_VS_STDLIB(" %02i ", 100);
    TEST_VS_STDLIB(" %03i ", 101);
    TEST_VS_STDLIB(" %01i ", -9);
    TEST_VS_STDLIB(" %02i ", -10);
    TEST_VS_STDLIB(" %03i ", -11);
    TEST_VS_STDLIB(" %04i ", -14);
    TEST_VS_STDLIB(" %05i ", -15);
    TEST_VS_STDLIB(" %06i ", -16);
    TEST_VS_STDLIB(" %01i ", -99);
    TEST_VS_STDLIB(" %02i ", -100);
    TEST_VS_STDLIB(" %03i ", -101);
    TEST_VS_STDLIB(" %09i ", INT_MAX);
    TEST_VS_STDLIB(" %010i ", INT_MIN);
    TEST_VS_STDLIB(" %011i ", LONG_MAX);
    TEST_VS_STDLIB(" %012i ", LONG_MIN);
    TEST_VS_STDLIB(" %013i ", UINT_MAX);
    TEST_VS_STDLIB(" %014i ", ULONG_MAX);
    TEST_VS_STDLIB(" %015i ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %09i %010i %011i %012i %013i %014i %015i", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %01u ", 0);
    TEST_VS_STDLIB(" %02u ", -1);
    TEST_VS_STDLIB(" %03u ", 1);
    TEST_VS_STDLIB(" %04u ", 9);
    TEST_VS_STDLIB(" %01u ", 10);
    TEST_VS_STDLIB(" %02u ", 11);
    TEST_VS_STDLIB(" %03u ", 15);
    TEST_VS_STDLIB(" %04u ", 16);
    TEST_VS_STDLIB(" %05u ", 17);
    TEST_VS_STDLIB(" %01u ", 99);
    TEST_VS_STDLIB(" %02u ", 100);
    TEST_VS_STDLIB(" %03u ", 101);
    TEST_VS_STDLIB(" %01u ", -9);
    TEST_VS_STDLIB(" %02u ", -10);
    TEST_VS_STDLIB(" %03u ", -11);
    TEST_VS_STDLIB(" %04u ", -14);
    TEST_VS_STDLIB(" %05u ", -15);
    TEST_VS_STDLIB(" %06u ", -16);
    TEST_VS_STDLIB(" %01u ", -99);
    TEST_VS_STDLIB(" %02u ", -100);
    TEST_VS_STDLIB(" %03u ", -101);
    TEST_VS_STDLIB(" %09u ", INT_MAX);
    TEST_VS_STDLIB(" %010u ", INT_MIN);
    TEST_VS_STDLIB(" %011u ", LONG_MAX);
    TEST_VS_STDLIB(" %012u ", LONG_MIN);
    TEST_VS_STDLIB(" %013u ", UINT_MAX);
    TEST_VS_STDLIB(" %014u ", ULONG_MAX);
    TEST_VS_STDLIB(" %015u ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %09u %010u %011u %012u %013u %014u %015u", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %01x ", 0);
    TEST_VS_STDLIB(" %02x ", -1);
    TEST_VS_STDLIB(" %03x ", 1);
    TEST_VS_STDLIB(" %04x ", 9);
    TEST_VS_STDLIB(" %01x ", 10);
    TEST_VS_STDLIB(" %02x ", 11);
    TEST_VS_STDLIB(" %03x ", 15);
    TEST_VS_STDLIB(" %04x ", 16);
    TEST_VS_STDLIB(" %05x ", 17);
    TEST_VS_STDLIB(" %01x ", 99);
    TEST_VS_STDLIB(" %02x ", 100);
    TEST_VS_STDLIB(" %03x ", 101);
    TEST_VS_STDLIB(" %01x ", -9);
    TEST_VS_STDLIB(" %02x ", -10);
    TEST_VS_STDLIB(" %03x ", -11);
    TEST_VS_STDLIB(" %04x ", -14);
    TEST_VS_STDLIB(" %05x ", -15);
    TEST_VS_STDLIB(" %06x ", -16);
    TEST_VS_STDLIB(" %01x ", -99);
    TEST_VS_STDLIB(" %02x ", -100);
    TEST_VS_STDLIB(" %03x ", -101);
    TEST_VS_STDLIB(" %09x ", INT_MAX);
    TEST_VS_STDLIB(" %010x ", INT_MIN);
    TEST_VS_STDLIB(" %011x ", LONG_MAX);
    TEST_VS_STDLIB(" %012x ", LONG_MIN);
    TEST_VS_STDLIB(" %013x ", UINT_MAX);
    TEST_VS_STDLIB(" %014x ", ULONG_MAX);
    TEST_VS_STDLIB(" %015x ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %09x %010x %011x %012x %013x %014x %015x", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %01X ", 0);
    TEST_VS_STDLIB(" %02X ", -1);
    TEST_VS_STDLIB(" %03X ", 1);
    TEST_VS_STDLIB(" %04X ", 9);
    TEST_VS_STDLIB(" %01X ", 10);
    TEST_VS_STDLIB(" %02X ", 11);
    TEST_VS_STDLIB(" %03X ", 15);
    TEST_VS_STDLIB(" %04X ", 16);
    TEST_VS_STDLIB(" %05X ", 17);
    TEST_VS_STDLIB(" %01X ", 99);
    TEST_VS_STDLIB(" %02X ", 100);
    TEST_VS_STDLIB(" %03X ", 101);
    TEST_VS_STDLIB(" %01X ", -9);
    TEST_VS_STDLIB(" %02X ", -10);
    TEST_VS_STDLIB(" %03X ", -11);
    TEST_VS_STDLIB(" %04X ", -14);
    TEST_VS_STDLIB(" %05X ", -15);
    TEST_VS_STDLIB(" %06X ", -16);
    TEST_VS_STDLIB(" %01X ", -99);
    TEST_VS_STDLIB(" %02X ", -100);
    TEST_VS_STDLIB(" %03X ", -101);
    TEST_VS_STDLIB(" %09X ", INT_MAX);
    TEST_VS_STDLIB(" %010X ", INT_MIN);
    TEST_VS_STDLIB(" %011X ", LONG_MAX);
    TEST_VS_STDLIB(" %012X ", LONG_MIN);
    TEST_VS_STDLIB(" %013X ", UINT_MAX);
    TEST_VS_STDLIB(" %014X ", ULONG_MAX);
    TEST_VS_STDLIB(" %015X ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %09X %010X %011X %012X %013X %014X %015X", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);


    TEST_VS_STDLIB(" %+d ", 0);
    TEST_VS_STDLIB(" %+d ", -1);
    TEST_VS_STDLIB(" %+d ", 9);
    TEST_VS_STDLIB(" %+d ", 10);
    TEST_VS_STDLIB(" %+d ", 11);
    TEST_VS_STDLIB(" %+d ", 15);
    TEST_VS_STDLIB(" %+d ", 16);
    TEST_VS_STDLIB(" %+d ", 17);
    TEST_VS_STDLIB(" %+d ", 99);
    TEST_VS_STDLIB(" %+d ", 100);
    TEST_VS_STDLIB(" %+d ", 101);
    TEST_VS_STDLIB(" %+d ", -9);
    TEST_VS_STDLIB(" %+d ", -10);
    TEST_VS_STDLIB(" %+d ", -11);
    TEST_VS_STDLIB(" %+d ", -14);
    TEST_VS_STDLIB(" %+d ", -15);
    TEST_VS_STDLIB(" %+d ", -16);
    TEST_VS_STDLIB(" %+d ", -99);
    TEST_VS_STDLIB(" %+d ", -100);
    TEST_VS_STDLIB(" %+d ", -101);
    TEST_VS_STDLIB(" %+d ", INT_MAX);
    TEST_VS_STDLIB(" %+d ", INT_MIN);
    TEST_VS_STDLIB(" %+d ", LONG_MAX);
    TEST_VS_STDLIB(" %+d ", LONG_MIN);
    TEST_VS_STDLIB(" %+d ", UINT_MAX);
    TEST_VS_STDLIB(" %+d ", ULONG_MAX);
    TEST_VS_STDLIB(" %+d ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %+d %+d %+d %+d %+d %+d %+d", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    TEST_VS_STDLIB(" %+i ", 0);
    TEST_VS_STDLIB(" %+i ", -1);
    TEST_VS_STDLIB(" %+i ", 1);
    TEST_VS_STDLIB(" %+i ", 9);
    TEST_VS_STDLIB(" %+i ", 10);
    TEST_VS_STDLIB(" %+i ", 11);
    TEST_VS_STDLIB(" %+i ", 15);
    TEST_VS_STDLIB(" %+i ", 16);
    TEST_VS_STDLIB(" %+i ", 17);
    TEST_VS_STDLIB(" %+i ", 99);
    TEST_VS_STDLIB(" %+i ", 100);
    TEST_VS_STDLIB(" %+i ", 101);
    TEST_VS_STDLIB(" %+i ", -9);
    TEST_VS_STDLIB(" %+i ", -10);
    TEST_VS_STDLIB(" %+i ", -11);
    TEST_VS_STDLIB(" %+i ", -14);
    TEST_VS_STDLIB(" %+i ", -15);
    TEST_VS_STDLIB(" %+i ", -16);
    TEST_VS_STDLIB(" %+i ", -99);
    TEST_VS_STDLIB(" %+i ", -100);
    TEST_VS_STDLIB(" %+i ", -101);
    TEST_VS_STDLIB(" %+i ", INT_MAX);
    TEST_VS_STDLIB(" %+i ", INT_MIN);
    TEST_VS_STDLIB(" %+i ", LONG_MAX);
    TEST_VS_STDLIB(" %+i ", LONG_MIN);
    TEST_VS_STDLIB(" %+i ", UINT_MAX);
    TEST_VS_STDLIB(" %+i ", ULONG_MAX);
    TEST_VS_STDLIB(" %+i ", 9223372036854775807LL);
    TEST_VS_STDLIB(" %+i %+i %+i %+i %+i %+i %+i", INT_MAX, INT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, 0, -42);

    // modified from https://opensource.apple.com/source/bash/bash-29/bash/tests/printf.tests.auto.html

    // this should expand escape sequences in the format string, nothing else
    TEST_VS_STDLIB("\tone\n");

    // this should not cut off output after the \c
    TEST_VS_STDLIB("one\ctwo\n");

    // and unrecognized backslash escapes should have the backslash preserverd
    TEST_VS_STDLIB("4\.2\n");

    TEST_VS_STDLIB("no newline ");
    TEST_VS_STDLIB("now newline \n");

    //%% -> %
    TEST_VS_STDLIB("%%\n");

    // this was a bug caused by pre-processing the string for backslash escapes
    // before doing the `%' format processing -- all versions before bash-2.04
#if ENABLE_INVALID_ARG_AMOUNT_TESTS
    TEST_DOES_NOT_CRASH("\045x");
    TEST_DOES_NOT_CRASH("\045d\n");
#endif

    // simple character output
    TEST_VS_STDLIB("%c\n", 'ABCD');
    // test simple string output
    TEST_VS_STDLIB("%s\n", "unquoted");

    // test quoted string output
    TEST_VS_STDLIB("%s %q\n", "unquoted", "quoted");
    TEST_VS_STDLIB("%s%10q\n", "unquoted", "quoted");

    TEST_VS_STDLIB("%q\n", "this&that");

    // make sure the format string is reused to use up arguments
    TEST_VS_STDLIB("%d ", 1, 2, 3, 4, 5);

#if ENABLE_INVALID_ARG_AMOUNT_TESTS
    // make sure that extra format characters get null arguments
    TEST_DOES_NOT_CRASH("%s %d %d %d\n", "onestring");
    TEST_DOES_NOT_CRASH("%s %d %u %4.2f\n", "onestring");

    // TEST_VS_STDLIB("--%s %s--\n", 4.2, '');
    // TEST_DOES_NOT_CRASH("--%s %s--\n", 4.2);
#endif

    // test %b escapes

    // 8 is a non-octal digit, so the `81' should be output
#if 0
	TEST_VS_STDLIB("--%b--\n", '\n\081');

	TEST_VS_STDLIB("--%b--\n", '\t\0101');
	TEST_VS_STDLIB("--%b--\n", '\t\101');

	// these should all display `A7'
	TEST_VS_STDLIB("%b\n", 01017);
	TEST_VS_STDLIB("%b\n", 1017);
	// TEST_VS_STDLIB("%b\n", '\x417');

	TEST_VS_STDLIB("--%b--\n", "\"abcd\"");
	TEST_VS_STDLIB("--%b--\n", "\'abcd\'");

	TEST_VS_STDLIB("--%b--\n", 'a\\x');

	TEST_VS_STDLIB("--%b--\n", '\\x');

	// TEST_VS_STDLIB("--%b--\n", '');
	TEST_VS_STDLIB("--%b--\n");

	// the stuff following the \c should be ignored, as well as the rest
	// of the format string
	// TEST_VS_STDLIB("--%b--\n", '4.2\c5.4\n');

	// unrecognized escape sequences should by displayed unchanged
	TEST_VS_STDLIB("--%b--\n", '4\.2');

	// a bare \ should not be processed as an escape sequence
	TEST_VS_STDLIB("--%b--\n", '\\');
#endif

    // make sure extra arguments are ignored if the format string doesn't
    // actually use them
    TEST_VS_STDLIB("\n", 4.4, "BSD");
    TEST_VS_STDLIB(" ", 4.4, "BSD");

    // make sure that a fieldwidth and precision of `*' are handled right
    TEST_VS_STDLIB("%10.8s\n", "4.4BSD");
    TEST_VS_STDLIB("%*.*s\n", 10, 8, "4.4BSD");

    TEST_VS_STDLIB("%10.8q\n", "4.4BSD");
    TEST_VS_STDLIB("%*.*q\n", 10, 8, "4.4BSD");

#if 0
	TEST_VS_STDLIB("%6b\n", "4.4BSD");
	TEST_VS_STDLIB("%*b\n", 6, "4.4BSD");

	// we handle this crap with homemade code in printf.def
	TEST_VS_STDLIB("%10b\n", "4.4BSD");
	TEST_VS_STDLIB("--%-10b--\n", "4.4BSD");
	TEST_VS_STDLIB("%4.2b\n", "4.4BSD");
	TEST_VS_STDLIB("%.3b\n", "4.4BSD");
	TEST_VS_STDLIB("--%-8b--\n", "4.4BSD");
#endif

    // test numeric conversions -- these four lines should echo identically
    TEST_VS_STDLIB("%d %u %i 0%o 0x%x 0x%X\n", 255, 255, 255, 255, 255, 255);
    TEST_VS_STDLIB("%d %u %i %#o %#x %#X\n", 255, 255, 255, 255, 255, 255);

    TEST_VS_STDLIB("%ld %lu %li 0%o 0x%x 0x%X\n", 255, 255, 255, 255, 255, 255);
    TEST_VS_STDLIB("%ld %lu %li %#o %#x %#X\n", 255, 255, 255, 255, 255, 255);

    TEST_VS_STDLIB("%10d\n", 42);
    TEST_VS_STDLIB("%10d\n", -42);

    TEST_VS_STDLIB("%*d\n", 10, 42);
    TEST_VS_STDLIB("%*d\n", 10, -42);

    // test some simple floating point formats
    TEST_VS_STDLIB("%4.2f\n", 4.2);
    TEST_VS_STDLIB("%#4.2f\n", 4.2);
    TEST_VS_STDLIB("%#4.1f\n", 4.2);

    TEST_VS_STDLIB("%*.*f\n", 4, 2, 4.2);
    TEST_VS_STDLIB("%#*.*f\n", 4, 2, 4.2);
    TEST_VS_STDLIB("%#*.*f\n", 4, 1, 4.2);

    TEST_VS_STDLIB("%E\n", 4.2);
    TEST_VS_STDLIB("%e\n", 4.2);
    TEST_VS_STDLIB("%6.1E\n", 4.2);
    TEST_VS_STDLIB("%6.1e\n", 4.2);

    TEST_VS_STDLIB("%G\n", 4.2);
    TEST_VS_STDLIB("%g\n", 4.2);
    TEST_VS_STDLIB("%6.2G\n", 4.2);
    TEST_VS_STDLIB("%6.2g\n", 4.2);

#if 0
	// test some of the more esoteric features of POSIX.1 printf
	TEST_VS_STDLIB("%d\n", "'string'");
	TEST_VS_STDLIB("%d\n", '"string"');

	TEST_VS_STDLIB("%#o\n", "'string'");
	TEST_VS_STDLIB("%#o\n", '"string"');

	TEST_VS_STDLIB("%#x\n", "'string'");
	TEST_VS_STDLIB("%#X\n", '"string"');

	TEST_VS_STDLIB("%6.2f\n", "'string'");
	TEST_VS_STDLIB("%6.2f\n", '"string"');
#endif

    // output from these two lines had better be the same
    TEST_VS_STDLIB("--%6.4s--\n", "abcdefghijklmnopqrstuvwxyz");
    // TEST_VS_STDLIB("--%6.4b--\n", "abcdefghijklmnopqrstuvwxyz");

    // and these two also
    TEST_VS_STDLIB("--%12.10s--\n", "abcdefghijklmnopqrstuvwxyz");
    // TEST_VS_STDLIB("--%12.10b--\n", "abcdefghijklmnopqrstuvwxyz");

    // tests for translating \' to ' and \\ to \

    TEST_VS_STDLIB("\'abcd\'\n");

#if 0
	// but not when the %b format specification is used
	TEST_VS_STDLIB("%b\n", 'abcd');

	// but both translate \\ to 
	TEST_VS_STDLIB('\\abcd\\\n');
	TEST_VS_STDLIB("%b\n", '\\abcd\\');
#endif

    // this was reported as a bug in bash-2.03
    // these three lines should all echo `26'
    TEST_VS_STDLIB("%d\n", 0x1a);
    TEST_VS_STDLIB("%d\n", 032);
    TEST_VS_STDLIB("%d\n", 26);

    // error messages

    // this should be an overflow, but error messages vary between systems
    TEST_VS_STDLIB("%lu\n", 4294967296);

    // ...but we cannot use this because some systems (SunOS4, for example),
    // happily ignore overflow conditions in strtol(3)
    TEST_VS_STDLIB("%ld\n", 4294967296);

    // in the future this may mean to put the output into VAR, but for
    // now it is an error
    // TEST_VS_STDLIB("%10d", $RANDOM);

    // TEST_VS_STDLIB("%10");
    // TEST_VS_STDLIB("ab%Mcd\n");

    // this caused an infinite loop in older versions of printf
    TEST_VS_STDLIB("%y", 0);

    // these should print a warning and `0', according to POSIX.2
    TEST_VS_STDLIB("%d\n", 'GNU');
    TEST_VS_STDLIB("%o\n", 'GNU');

    // failures in all bash versions through bash-2.05
    TEST_VS_STDLIB("%.0s", "foo");
    TEST_VS_STDLIB("%.*s", 0, "foo");

    TEST_VS_STDLIB("%.0s-%.0s\n", "foo", "bar");
    TEST_VS_STDLIB("(%*s)(%*s)\n", -1, "foo", -2, "bar");

    TEST_VS_STDLIB("%0100384d", 0);
}