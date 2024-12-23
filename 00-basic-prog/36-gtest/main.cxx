#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(gtest_36, basic_assertions)
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}
