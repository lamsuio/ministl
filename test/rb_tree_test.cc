#include <stdio.h>
#include <gtest/gtest.h>
#include "../ministl/rb_tree.h"
USE_MINISTL;

TEST(RBTreeTest, PushTest) {
    rb_tree<int, int> tree;
    tree.push(1);
}


