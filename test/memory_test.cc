#include <cstdio>
#include <gtest/gtest.h>
#include "../ministl/memory.h"

USE_MINISTL;

TEST(MemoryTest, AllocTest) {
    int *a, *b, *c, *d;
    a = (int *)mini_alloc<int>::allocate(10);
    b = (int *)mini_alloc<int>::allocate(10);
    c = (int *)mini_alloc<int>::allocate();
    d = c;
    EXPECT_EQ((void*)*a, b);
    for(int i = 0; i< 10; i++){
        a[i] = i;
        b[i] = i + 10;
    }
    mini_alloc<int>::deallocate(a, 10);
    mini_alloc<int>::deallocate(b, 10);
    a = (int *)mini_alloc<int>::allocate(10); // Now a was pointer to b data
    for(int i = 4; i< 10; i++){
        EXPECT_EQ(a[i], 10+i);
    }
    mini_alloc<int>::deallocate(a, 10);
    mini_alloc<int>::deallocate(c);
    a = (int *)mini_alloc<int>::allocate();
    b = (int *)mini_alloc<int>::allocate();
    EXPECT_TRUE(a == d);
    mini_alloc<int>::deallocate(b);
    mini_alloc<int>::deallocate(a);
}
