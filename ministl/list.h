#pragma once

#include <cstddef>
#include "common.h"
#include "memory.h"
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
    list_iterator()  {}
    list_iterator(const iterator& x) : node(x.node) {}

    bool operator==(const self& other) const { return other.node == node; }
    bool operator!=(const self& other) const { return other.node != node; }
    reference operator*() const { return node->value; }

    // TODO: Standard member access operator
    pointer  operator->() const { return &(operator*()); }

    self& operator++() { node = node->next; return *this; }
    self operator++(int) { self tmp = *this; ++(*this); return tmp; }

    self&  operator--() { node = node->prev; return *this; }
    self   operator--(int) { self tmp = *this; --(*this); return tmp; }
};

template<class T, class Alloc=alloc>
class list {
    public:
        typedef list_node<T> * link_type;
        typedef list_iterator<T, T&, T*> iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T& reference;
        typedef T* pointer;

    protected:
        link_type node;

    public:
        iterator begin() const { return node->next; }
        iterator end() const { return node; }
        bool  empty() const { return begin() == end();}
        size_type size() const ;

        reference front() { return *begin(); }
        reference back() { return *(--end());}

    protected:
        typedef mini_alloc<list_node<T>, Alloc> list_node_allocator;

        link_type get_node() {return (link_type)list_node_allocator::allocate(); } // create memory space
        void put_node(link_type p) { list_node_allocator::deallocate(p); } // release node p
        link_type create_node(const T& x); // create a node with x value
        void destroy_node(link_type p); // destroy a node

        void empty_initialize();  //create a empty list;

    public:
        list(){ empty_initialize(); }
        void push_front(const T& x);
        void push_back(const T& x);
        iterator insert(iterator position, const T& x);
        iterator erase(iterator postion);
        iterator erase(iterator start, iterator last);
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

template<class T, class Alloc>
void list<T, Alloc>::destroy_node(typename list<T, Alloc>::link_type p) {
    destroy(&p->value);
    put_node(p);
}

template<class T, class Alloc>
typename list<T, Alloc>::link_type
list<T, Alloc>::create_node(const T& value) {
    link_type p = get_node();
    construct(&p->value, value);
    return p;
}

template<class T, class Alloc>
void list<T, Alloc>::empty_initialize() {
    this->node = get_node();
    this->node->next = this->node;
    this->node->prev = this->node;
}

template<class T, class Alloc>
void list<T, Alloc>::push_front(const T& x) {
    link_type p = create_node(x);
    p->next = this->node->next;
    p->prev = this->node;
    this->node->next = p;
    p->next->prev = p;
}

template<class T, class Alloc>
void list<T, Alloc>::push_back(const T& x) {
    link_type p = create_node(x);
    p->prev = this->node->prev;
    p->next = this->node;
    p->prev->next = p;
    this->node->prev = p;
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::erase( typename list<T,Alloc>::iterator position) {
    link_type next = position.node->next;
    link_type tmp  = position.node;
    tmp->prev->next = tmp->next;
    tmp->next->prev = tmp->prev;
    destroy_node(position.node);
    return iterator(next);
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator
list<T, Alloc>::erase(typename list<T, Alloc>::iterator start,
        typename list<T, Alloc>::iterator last) {
    iterator tmp = start;
    last.node->prev = start.node->prev;
    start.node->prev->next = last.node;

    while (start != last) {
        tmp = start;
        ++start;
        destroy_node(tmp.node);
    }

    return last;
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator
list<T,Alloc>::insert(typename list<T, Alloc>::iterator position,
        const T& x) {
    link_type p = create_node(x);
    p->next = position.node->next;
    position.node->next = p;
    p->prev = position.node;
    p->next->prev = p;
    return iterator(p);
}

template<class T, class Alloc>
void list<T, Alloc>::clear(){
    iterator start = begin();
    iterator last  = end();
    erase(start, last);
    node->next = node;
    node->prev = node;
}

template<class T, class Alloc>
T& list<T, Alloc>::pop_front() {
    iterator it = erase(begin());
    return *it;
}

template<class T, class Alloc>
T& list<T, Alloc>::pop_back() {
    iterator tmp = end();
    return *erase(--tmp);
}

template<class T, class Alloc>
typename list<T, Alloc>::size_type
list<T, Alloc>::size() const {
    iterator tmp = begin();
    size_type length = 0;
    while (tmp != end()) {
        ++length;
        ++tmp;
    }
    return length;
}

END_MINISTL
