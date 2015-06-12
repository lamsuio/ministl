#include <stdio.h>
#include <gtest/gtest.h>
#include "../ministl/avl_tree.h"
USE_MINISTL;
TEST(AVLTreeTest, PushTest) {
    avl_tree<int> tree;
    tree.push(14);
    printf("%d\n", tree.top()->value);
    tree.push(16);
    EXPECT_TRUE(tree.top()->left == NULL);
    EXPECT_EQ(tree.top()->right->value, 16);
    tree.push(15);
    EXPECT_EQ(tree.top()->value, 16);
}

