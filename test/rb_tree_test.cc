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
}


