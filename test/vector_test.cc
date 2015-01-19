#include "gtest/gtest.h"
#include "../ministl/vector.h"

USE_MINISTL;

TEST(VectorTest, EmptyVector) {
    vector<int> vec;
    EXPECT_TRUE(vec.begin() == NULL);
    EXPECT_TRUE(vec.end() == NULL);
    EXPECT_EQ(vec.size(), size_t(0));
    EXPECT_EQ(vec.capacity(), size_t(0));
}
