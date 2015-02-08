#include <unistd.h>
#include <sys/resource.h>
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

TEST(VectorTest, OneVector) {
    vector<int> vec(10, 1);
    EXPECT_TRUE(vec.size() == 10);
    EXPECT_TRUE(vec.capacity() == 10);
    EXPECT_TRUE(vec[0] == 1);
    EXPECT_TRUE(vec[1] == 1);
    EXPECT_TRUE(vec[9] == 1);
}

TEST(VectorTest, PushElement) {
    vector<int> vec(10, 1);
    vec.push_back(2);
    EXPECT_TRUE(vec.size()== 11);
    EXPECT_TRUE(vec.capacity() == 20);
    EXPECT_TRUE(vec[10] == 2);

    vector<int> vec2(2, 1);
    vec2.push_back(3);
    EXPECT_TRUE(vec2.size() == 3);
    EXPECT_TRUE(vec2.capacity() == 8);
    EXPECT_TRUE(vec2[2] == 3);
}

TEST(VectorTest, PushEmpty) {
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    vec.push_back(6);
    vec.push_back(7);
    vec.push_back(8);
    vec.push_back(9);
    EXPECT_TRUE(vec.size() == 9);
    EXPECT_TRUE(vec.capacity() == 16);
    EXPECT_EQ(vec[8], 9);
}

TEST(VectorTest, PushPop) {
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    EXPECT_TRUE(vec.size() == 2);
    EXPECT_TRUE(vec.capacity() == 8);
    vec.pop_back();
    EXPECT_TRUE(vec.size() == 1);
    EXPECT_TRUE(vec.capacity() == 8);
    vec.push_back(100);
    EXPECT_TRUE(vec[1] == 100);
}

TEST(VectorTest, MemoryLeak) {
    int i;
    vector<int> v;
    for (i = 0; i < 1000; i++) {
        vector<int> vec(1000);
        vec.push_back(1);
    }
    EXPECT_TRUE(v.size()==0);
}
