#include <cstdlib>
#include <gtest/gtest.h>
#include "../ministl/rb_tree.h"
USE_MINISTL;

void fulfillTree(rb_tree<int>& tree, int size) {
    for (auto i = 0; i < size; i++) {
        tree.insert(rand());
    }
}

TEST(RBTreeTest, PushTest) {
    rb_tree<int> tree;
	tree.insert(1);
	tree.insert(2);
	tree.insert(3);
	tree.insert(5);
	tree.insert(0);
	tree.insert(-1);
	auto first = tree.begin();
	EXPECT_EQ(-1, *first);
	EXPECT_EQ(0, *(++first));
	EXPECT_EQ(1, *(++first));
	EXPECT_EQ(2, *(++first));
	EXPECT_EQ(3, *(++first));
	EXPECT_EQ(5, *(++first));
    ++first;
    EXPECT_TRUE(tree.end() == first);
}

TEST(RBTreeTest, SizeTest) {
    rb_tree<int> tree;
    for (auto i = 1; i < 1000; i++) {
        tree.clear();
        fulfillTree(tree, i);
        EXPECT_EQ(tree.size(), (size_t)i);
    }
}

TEST(RBTreeTest, ClearTest) {
    rb_tree<int> tree;
    fulfillTree(tree, 1000);
    tree.clear();
    EXPECT_TRUE(tree.begin() == tree.end());
}

TEST(RBTreeTest, InsreaseIteratorTest) {
    rb_tree<int> tree;
    fulfillTree(tree, 1000);

    auto prev = *tree.begin();
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        EXPECT_TRUE(prev <= *it);
        prev = *it;
    }
}

TEST(RBTreeTest, DereaseIteratorTest) {
    rb_tree<int> tree;
    fulfillTree(tree, 1000);

    auto prev = 0;
    auto it = tree.end();
    do {
        if (it-- == tree.end()) {
            prev = *it;
        }
        EXPECT_TRUE(prev >= *it);
    } while(it != tree.begin());
}


TEST(RBTreeTest, RBTreePropertiesTest) {
    rb_tree<int> tree;

    __rb_test_helper<rb_tree<int>> helper(&tree);

    for (int i = 0; i < 1000; i++) {
        fulfillTree(tree, 1000);

        EXPECT_TRUE(helper.rule_1() == tree.end());
        EXPECT_TRUE(helper.rule_2() == tree.end());
        EXPECT_TRUE(helper.rule_3() == tree.end());
        EXPECT_TRUE(helper.rule_4() == tree.end());
        EXPECT_TRUE(helper.rule_5() == tree.end());

        tree.clear();
    }
}

TEST(RBTreeTest, SearchTest) {
    rb_tree<int> tree;
    fulfillTree(tree, 1000);
}
