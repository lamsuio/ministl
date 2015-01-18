#include <cstdio>
#include "gtest/gtest.h"
#include "../ministl/memory.h"

USE_MINISTL;

TEST(MemoryTest, AllocTest) {
   int *a, *b, *c;
   a = (int *)mini_alloc<int>::allocate(10);
   b = (int *)mini_alloc<int>::allocate();
   c = b;
   mini_alloc<int>::deallocate(b);
   mini_alloc<int>::deallocate(a, 10);
   a = (int *)mini_alloc<int>::allocate();
   b = (int *)mini_alloc<int>::allocate();
   EXPECT_TRUE((int *)(*a) == b);
   EXPECT_TRUE(a == c);
   mini_alloc<int>::deallocate(b);
   mini_alloc<int>::deallocate(a);
} 
