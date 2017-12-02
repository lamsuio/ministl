#pragma once

#include <cstddef>
#include "common.h"
#include "iterator.h"
#include "memory.h"
#include "algorithm.h"

START_MINISTL

template<class T, class Alloc = alloc>
class vector
{
public:
    typedef T           value_type;
    typedef value_type* iterator;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

private:
    static const char bit_per_word = 8;

    static inline size_type ROUND_UP(size_type new_size) {
        return (new_size + (bit_per_word - 1))& ~(bit_per_word - 1);
    } 

protected:
    iterator    start;
    iterator    finish;
    iterator    end_of_storage;
    typedef     mini_alloc<T, Alloc> data_allocate;

    void insert_aux(iterator position, const T& value);

    
    void deallocate() {
        if (start) {
            data_allocate::deallocate(start, end_of_storage - start);
        }
    }

    void fill_initialize(size_type n, const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

    void copy_forward(iterator dst, iterator src_start, iterator src_end) {
        dst += src_end - src_start;
        while (src_end != src_start) {
            *--dst = *--src_end;//TODO:
        }
    }

    void copy_backward(iterator dst, iterator src_start, iterator src_end) {
        while (src_end != src_start) {
            *dst = *src_start;
            ++dst;
            ++src_start;
        }
    }

    iterator allocate_and_fill(size_type count, const T& value);

public:
    iterator begin()    const { return start; }
    iterator end()      const { return finish; }
    size_t   size()     const { return size_type(end() - begin()); }
    size_t   capacity() const { return size_type(end_of_storage - begin()); }
    bool     empty()    const { return begin() == end(); }
    T&       operator[](size_t index) { return *(start + index); }

public:
    vector(): start(0), finish(0), end_of_storage(0) { }
    vector(size_t count, const T& value) { fill_initialize(count, value); }
    vector(int    count, const T& value) { fill_initialize(count, value); }
    vector(long   count, const T& value) { fill_initialize(count, value); }
    explicit vector(size_t count) { fill_initialize(count, T()); } 

    ~vector() {
        destroy<T>(start, finish);
        deallocate();
    }

    T& front() const { return *begin(); }
    T& back()  const { return *(end() - 1); }

    void push_back(const T& value) {
        if (finish != end_of_storage) {
            construct(end(), value);
            ++finish;
        }
        else {
            // full and need expand
            insert_aux(end(), value);
        }
    }

    void pop_back() {
        --finish;
        destroy<T>(finish);
    }

};

// Allocate and fill with value given
//
template<class T, class Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::allocate_and_fill(typename vector<T, Alloc>::size_type count, 
        const T& value) {
    iterator pointer = (iterator)data_allocate::allocate(count);
    for (size_type i = 0; i < count; i++) {
        construct(pointer + i, value);
    }

    return pointer;
}

template<class T, class Alloc>
void vector<T, Alloc>::insert_aux(typename vector<T, Alloc>::iterator position, 
        const T& value) {
    if (finish < end_of_storage) {
        if (finish != position) {
            copy_forward(position + 1, position, finish);
        }
        construct(position, value);
        this->finish++;
    } 
    else {
        // Need to expand
        size_t new_size = max( capacity()*2, ROUND_UP(size() + 1));
        //TODO:
        iterator new_start = allocate_and_fill(new_size, T());
        iterator new_end = new_start;
        iterator pointer = start;
        while(pointer != finish) {
            *new_end = *pointer;
            new_end++;
            pointer++;
        }
        destroy<T>(start, finish);
        data_allocate::deallocate(start, size());
        construct(new_end, value);
        start = new_start;
        finish = new_end+1;
        end_of_storage = start + new_size;
    }
}

END_MINISTL
