#include <gtest/gtest.h>

class gtest_36 : public testing::Test
{
protected:
    gtest_36()
        : v(5)
    {
        // "vectors can be sized and resized"
        EXPECT_EQ(v.size(), 5);
        EXPECT_GE(v.capacity(), 5);
    }

    std::vector<int> v;
};

// adding to the vector increases its size
TEST_F(gtest_36, adding_to_the_vector_increases_its_size)
{
    v.push_back(1);

    EXPECT_EQ(v.size(), 6);
    EXPECT_GE(v.capacity(), 6);
}

TEST_F(gtest_36, reserving_increases_just_the_capacity)
{
    v.reserve(6);

    EXPECT_EQ(v.size(), 5);
    EXPECT_GE(v.capacity(), 6);
}
