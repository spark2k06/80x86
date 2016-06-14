#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"

template <typename T1, typename T2>
struct DivTest {
    T1 v1;
    T2 v2;
    T2 quotient;
    T2 remainder;
};

using Div8Params = std::pair<const std::vector<uint8_t>,
      const std::vector<struct DivTest<uint16_t, uint8_t>>>;
using Div16Params = std::pair<const std::vector<uint8_t>,
      const std::vector<struct DivTest<uint32_t, uint16_t>>>;

static const std::vector<struct DivTest<uint16_t, uint8_t>> div8_tests = {
    { 100, 20, 5, 0 },
    { 500, 250, 2, 0 },
    { 10, 3, 3, 1 },
};

class DivReg8Test : public EmulateFixture,
    public ::testing::WithParamInterface<Div8Params> {
};
TEST_P(DivReg8Test, Result)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " / " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AX, t.v1);
        write_reg(BL, t.v2);
        // xDIV bl
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.quotient);
        ASSERT_EQ(read_reg(AH), t.remainder);
    }
}
INSTANTIATE_TEST_CASE_P(Div, DivReg8Test,
    ::testing::Values(
        Div8Params({ 0xf6, 0xf3 }, div8_tests)
    ));

class DivMem8Test : public EmulateFixture,
    public ::testing::WithParamInterface<Div8Params> {
};
TEST_P(DivMem8Test, Result)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " / " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AX, t.v1);
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.v2);

        // xDIV byte [bx]
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.quotient);
        ASSERT_EQ(read_reg(AH), t.remainder);
    }
}
INSTANTIATE_TEST_CASE_P(Div, DivMem8Test,
    ::testing::Values(
        Div8Params({ 0xf6, 0x37 }, div8_tests)
    ));

static const std::vector<struct DivTest<uint32_t, uint16_t>> div16_tests = {
    { 1000, 200, 5, 0 },
    { 500, 250, 2, 0 },
    { 1000, 3, 333, 1 },
};

class DivReg16Test : public EmulateFixture,
    public ::testing::WithParamInterface<Div16Params> {
};
TEST_P(DivReg16Test, Result)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " / " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(DX, (t.v1 >> 16) & 0xffff);
        write_reg(AX, t.v1);
        write_reg(BX, t.v2);
        // xDIV bx
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.quotient);
        ASSERT_EQ(read_reg(DX), t.remainder);
    }
}
INSTANTIATE_TEST_CASE_P(Div, DivReg16Test,
    ::testing::Values(
        Div16Params({ 0xf7, 0xf3 }, div16_tests)
    ));

class DivMem16Test : public EmulateFixture,
    public ::testing::WithParamInterface<Div16Params> {
};
TEST_P(DivMem16Test, Result)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " / " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(DX, (t.v1 >> 16) & 0xffff);
        write_reg(AX, t.v1);
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.v2);

        // xDIV word [bx]
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.quotient);
        ASSERT_EQ(read_reg(DX), t.remainder);
    }
}
INSTANTIATE_TEST_CASE_P(Div, DivMem16Test,
    ::testing::Values(
        Div16Params({ 0xf7, 0x37 }, div16_tests)
    ));