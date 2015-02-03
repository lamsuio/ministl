#pragma once

#include <cstddef>
#include "common.h"
#include "iterator.h"

START_MINISTL

template<class T>
struct list_node {
    struct list_node<T> * prev;
    struct list_node<T> * next;
    T  value;
};

template<class T, class Ref, class Pointer>
struct list_iterator {
    typedef list_iterator<T, T&, T*> iterator;
    typedef list_iterator<T, Ref, Pointer> self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ref  reference ;
    typedef Pointer pointer;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef list_node<T>* link_type;

    link_type node; // current link node

    list_iterator(link_type n) : node(n) {}
    list_iterator() : {}
    list_iterator(const iterator& x) : node(x.node) {}

    bool operator==(const self& other) const { return other.node == node; }
    bool operator!=(const self& other) const { return other.node != node; }
    reference operator*() const { return (*node).value; }

    // TODO: Standard member access operator
    pointer  operator->() const { return &(operator*()); }

    reference operator++() { node = node->next; return *this; }
    reference operator++(int) { self tmp = *this; ++(*this); return tmp; }

    reference operator--() { node = node->prev; return *this; }
    reference operator--(int) { self tmp = *this; --(*this); return tmp; }
};

template<class T, class Alloc=alloc>
class list {
    public:
        typedef list_node<T> * link_type;
        typedef list_iterator<T, T&, T*> iterator;
        typedef iterator::size_type size_type;
        typedef iterator::diifference_type difference_type;
        typedef T& reference;
        typedef T* pointer;

    protected:
        link_type node;

    public:
        iterator begin() const { return node->next; }
        iterator end() const { return node; }
        bool  empty() const { return begin() == end();}
        size_type size() const;// TODO:

        reference front() { return *begin(); }
        reference back() { return *(--end());}

    protected:
        typedef mini_alloc<list_node<T>, Alloc> list_node_allocator;

        link_type get_node(); // TODO: create memory space
        void put_node(link_type p); //TODO: release node p
        link_type create_node(const T& x); //TODO: create a node with x value
        void destroy(link_type p); // TODO: destroy a node

        void empty_initialize();  //TODO: create a empty list;

    public:
        void push_front(const T& x);
        void push_back(const T& x);
        iterator insert(iterator position, const T& x);
        void erase(iterator postion);
        void erase(iterator start, iterator last);
        T& pop_back();
        T& pop_front();
        void clear();
        T& remove(iterator position);
        void splice(iterator position, iterator start, iterator last);
        void splice(iterator position, list* other);
        void merge(list* other);
        void reverse();
        void sort();
};

END_MINISTL
