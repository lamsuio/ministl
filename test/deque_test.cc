#ifdef _MSC_VER
//#include <vld.h> 
#endif
#include "gtest/gtest.h"
#include "../ministl/deque.h"

USE_MINISTL;

// TEST FOR DEQUE_ITERATOR

TEST(DequeIteratorTest, BasicTest) {
    deque_iterator<int, int&, int*, 32> it;

}

TEST(Deque, BasicTest) {
    deque<int> deq(20, 9);
    EXPECT_EQ(deq.size(), 20);
    for(auto s = deq.begin(); s != deq.end(); ++s) {
        EXPECT_EQ(*s, 9);
    }
    
    for(auto i = 0; i < deq.size(); i++) {
        deq[i] = i;
    }
    
    for(auto i = 0; i < deq.size(); i++) {
        EXPECT_EQ(deq[i], i);
    }
    
    for(auto i = 0; i < 3; i++) {
        deq.push_back(i);
    }
    EXPECT_EQ(deq.size(), 23);
    for(auto i = 0; i < 3; i++) {
        EXPECT_EQ(deq[i+20], i);
    }
    
    deq.push_front(99);
    EXPECT_EQ(deq.size(), 24);
    EXPECT_EQ(deq.front(), 99);
    
    deq.push_front(98);
    deq.push_front(97);
    EXPECT_EQ(deq.size(), 26);
    EXPECT_EQ(deq.front(), 97);
    EXPECT_EQ(deq.back(), 2);
}
