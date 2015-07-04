#include <stdio.h>
#include <gtest/gtest.h>
#include "../ministl/rb_tree.h"
USE_MINISTL;

TEST(RBTreeTest, PushTest) {
    rb_tree<int> tree;
	tree.push(1);
	tree.push(2);
	tree.push(3);
	tree.push(5);
	tree.push(0);
	tree.push(-1);
	auto first = tree.begin();
	EXPECT_EQ(-1, *first);
	EXPECT_EQ(0, *(++first));
	EXPECT_EQ(1, *(++first));
	EXPECT_EQ(2, *(++first));
	EXPECT_EQ(3, *(++first));
	EXPECT_EQ(5, *(++first));
}


