#pragma once

#include "common.h"
#include "memory.h"
#include "iterator.h"

START_MINISTL

inline size_t __deque_buf_size(size_t n, size_t sz) {
    return n!= 0 ? n : (sz < 512 ? size_t(512/sz) : size_t(1));
}

template<class T, class Ref, class Pointer, size_t BufSize>
struct deque_iterator {
    // do some stuff
    typedef deque_iterator<T, T&, T*, BufSize> iterator;
    typedef deque_iterator<T, const T&, const T*, BufSize> const_iterator;
    typedef deque_iterator self;
    static size_t buffer_size(){ return __deque_buf_size(BufSize, sizeof(T)); }

    typedef random_access_iterator_tag iterator_category;
    typedef T       value_type;
    typedef size_t  size_type;
    typedef Pointer pointer;
    typedef Ref     reference;
    typedef ptrdiff_t difference_type;
    typedef T**     map_pointer;

    // Keep connection with deque container
    T* cur;     // pointer to current element
    T* first;   // pointer to buffer head
    T* last;    // pointer to buffer tail
    map_pointer node;   // pointer to map

    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }

    reference operator*() const { return *cur; }
    pointer  operator->() const { return &(operator*()); }
    difference_type operator-(const self& that) const {
        return difference_type(buffer_size()) * (node - that.node - 1) + // the nodes between this and that
            (cur - first) + (that.last - that.cur);   // add first node and last node
    }

    self& operator++() {
        ++cur;
        if (cur == last) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self& operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        if (cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self& operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    // Random access
    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < difference_type(buffer_size())) {
            // within current node
            cur += n;
        }else {
            // we need to move to forward or backward nodes
            difference_type node_offset =
                offset > 0 ? offset / difference_type(buffer_size())  // move forward nodes
                           : -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            // mode cur to correct
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n) { return *this += -n; }

    self operator-(difference_type n) const {
        self tmp  = *this;
        return tmp -= n;
    }

    // random read implementation
    reference operator[](difference_type n) const { return *(*this + n); }

    bool operator==(const self& that) const { return cur == that.cur; }
    bool operator!=(const self& that) const { return !(*this == that); }
    bool operator<(const self& that)  const {
        return (node == that.node) ? (cur < that.node) : (node < that.node);
    }
};

template<class T, class Alloc = alloc, size_t BufSize = 0>
class deque {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef deque_iterator<T, T&, T*, BufSize> iterator;
        typedef T& referece;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    protected:
        typedef pointer* map_pointer;

    protected:
        map_pointer map;
        size_type map_size;

    public:
        deque () {}
        deque (int buffer_size) {}
};

END_MINISTL


