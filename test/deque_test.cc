#include "gtest/gtest.h"
#include "../ministl/deque.h"

USE_MINISTL;

// TEST FOR DEQUE_ITERATOR

TEST(DequeIteratorTest, BasicTest) {
    deque_iterator<int, int&, int*, 32> it;

}
