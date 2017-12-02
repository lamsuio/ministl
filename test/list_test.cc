#include "gtest/gtest.h"
#include "../ministl/list.h"

USE_MINISTL;

TEST(ListTest, NewTest) {
    list<int> l;
    EXPECT_TRUE(l.empty());
}

TEST(ListTest, PutTest) {
    list<int> l;
    l.push_front(1);
    EXPECT_EQ(l.front(), 1);
    EXPECT_EQ(l.back(), 1);
    l.push_front(2);
    EXPECT_EQ(l.front(), 2);
    EXPECT_EQ(l.back(), 1);
    l.push_back(3);
    EXPECT_EQ(l.size(), 3u);
    EXPECT_EQ(l.front(), 2);
    EXPECT_EQ(l.back(), 3);
    list<int>::iterator it = l.begin();
    EXPECT_EQ(*it, 2);it++;
    EXPECT_EQ(*it, 1);it++;
    EXPECT_EQ(*it, 3);it++;
    EXPECT_TRUE(it == l.end());
}

struct TestObject{
    char a;
    int b;
};

TEST(ListTest, ClassTest) {
    int count= 10000;
    int result= 0;
    for (int i=0; i< count; i++) {
        list<TestObject> tmp;
        tmp.push_front(TestObject{1,2});
        tmp.push_front(TestObject{1,3});
        tmp.push_front(TestObject{1,4});
        tmp.push_front(TestObject{1,5});
        result += tmp.size();
    }
    EXPECT_EQ(result, 4*count);
}
