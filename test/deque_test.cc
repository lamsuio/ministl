#ifdef _MSC_VER
//#include <vld.h> 
#endif
#include "gtest/gtest.h"
#include "../ministl/deque.h"

USE_MINISTL;

// TEST FOR DEQUE_ITERATOR

TEST(DequeIteratorTest, BasicTest) {
    deque_iterator<int, int&, int*, 32> it __attribute__((unused));
}

TEST(Deque, BasicTest) {
    deque<int> deq(20, 9);
    EXPECT_EQ(deq.size(), 20u);
    for(auto s = deq.begin(); s != deq.end(); ++s) {
        EXPECT_EQ(*s, 9);
    }
    
    for(auto i = 0u; i < deq.size(); i++) {
        deq[i] = i;
    }
    
    for(auto i = 0u; i < deq.size(); i++) {
        EXPECT_EQ(deq[i], (int)i);
    }
    
    for(auto i = 0; i < 3; i++) {
        deq.push_back(i);
    }
    EXPECT_EQ(deq.size(), 23u);
    for(auto i = 0; i < 3; i++) {
        EXPECT_EQ(deq[i+20], i);
    }
    
    deq.push_front(99);
    EXPECT_EQ(deq.size(), 24u);
    EXPECT_EQ(deq.front(), 99);
    
    deq.push_front(98);
    deq.push_front(97);
    EXPECT_EQ(deq.size(), 26u);
    EXPECT_EQ(deq.front(), 97);
    EXPECT_EQ(deq.back(), 2);
}
