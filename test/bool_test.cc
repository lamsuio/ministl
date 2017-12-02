#include <stdio.h>

#include "gtest/gtest.h"
#include "../ministl/bool.h"

using namespace minstl;

TEST(MinSTLTest, BoolTest)
{
    EXPECT_EQ(Bool(-1), true);
    EXPECT_EQ(Bool(10), true);
    EXPECT_EQ(Bool(0), false);

    EXPECT_EQ(int(Bool(-1)), 1);
    EXPECT_EQ(int(Bool(10)), 1);
    EXPECT_EQ(int(Bool(0)), 0);
    EXPECT_EQ(sizeof(Bool(0)), 1u);

    EXPECT_TRUE(Bool(-1));
    EXPECT_FALSE(Bool(0));
    EXPECT_TRUE(Bool(10));
}

