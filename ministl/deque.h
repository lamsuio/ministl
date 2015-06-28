#pragma once

#include "common.h"
#include "memory.h"
#include "iterator.h"
#include "algorithm.h"

START_MINISTL

//
// Deque 的设计理念
// 一个T**数组，每个元素指向一个固定大小的T[]，这个大小由BufSize决定
//

//
// 返回 deque 的 buffer 大小
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

    self operator++(int) {
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

    self operator--(int) {
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
        typedef T& reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    protected:
        typedef pointer* map_pointer;
        typedef mini_alloc<value_type, Alloc> data_allocator;
        typedef mini_alloc<pointer, Alloc>    map_allocator;

    protected:
        iterator first; // 第一个元素
        iterator last;  // 最后一个元素的下一个 [first, last)
        map_pointer map; 
        size_type map_size;
        
    protected:        
        void fill_initialize(size_type n, const T& val);
        void create_map_and_nodes(size_type n); // 负责构造deque结构 
        static size_type buffer_size() { return iterator::buffer_size(); }
        size_type initial_map_size() { return size_type(8); } // 最小管理单元
        pointer allocate_node() { return (pointer)data_allocator::allocate(buffer_size()); }
		void deallocate_data(pointer data) { // 释放每个node 指向的区域
			size_type size = buffer_size();
			for (auto i = 0; i < size; i++){
				destroy(&(*(data + i)));
			}
			data_allocator::deallocate(data, size);
		}
        void deallocate_node(map_pointer node, size_type size) {// 释放整片map 
            map_allocator::deallocate(node, size);
        }
        void reallocate_map(size_type size_to_add, bool is_front);
        
        void reserve_map_at_back(size_type size_to_add = 1) {
            if(size_to_add + 1 > map_size - (last.node - map)) {
                reallocate_map(size_to_add, false);
            }
        }
        
        void reserve_map_at_front(size_type size_to_add = 1) {
            if(size_to_add > (first.node - map)) {
                reallocate_map(size_to_add, false);
            }
        }
        
        void push_back_aux(const T& t);
        
        void push_front_aux(const T& t);
        
        void pop_back_aux();
        
        void pop_front_aux();
        
    public:
        deque (): first(), last(), map(nullptr), map_size(0) {
            fill_initialize(0, T());
        }
        deque (size_t size): first(), last(), map(nullptr), map_size(0) {
            fill_initialize(size, T());
        }
        deque (size_type n, const T& val)
            : first(), last(), map(nullptr), map_size(0) {
            fill_initialize(n, val);
        }
        
        virtual ~deque() {
			map_pointer cur;
			for (cur = first.node; cur <= last.node; ++cur) {
				deallocate_data(*cur);
			}
			deallocate_node(map, map_size);
        }
        
    public:
        iterator begin() { return first; }
        iterator end() { return last; }
        reference operator[] (size_type n ){
            return first[difference_type(n)];
        }
        
        reference front() { return *first; }
        reference back() {
            iterator tmp = last;
            --tmp;
            return *tmp;
        }
        
        size_type size() { return last - first; } // TODO: STL 源码剖析里头这里有两个 ;; 为什么？
        size_type max_size() { return size_type(-1); }
        
        bool empty() const { return first == last; }
        
    public:
        void push_back(const T& t) {
            if (last.cur != last.last - 1) {
                construct(last.cur, t);
                ++last.cur;
            }
            else {
                push_back_aux(t); // 需要扩容
            }
        }
        
        void push_front(const T& t) {
            if (first.cur != first.first) {
                --first.cur;
                construct(first.cur, t);
            }
            else {
                push_front_aux(t);
            }
        }
};

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n,
    const T& val) {
    create_map_and_nodes(n);
    map_pointer cur;
    for(cur = first.node; cur < last.node; ++cur) {
        uninitialized_fill(*cur, *cur + buffer_size(), val); // 给每个node初始化值
    }
    uninitialized_fill(last.first, last.cur, val);
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type n) {
    size_t node_count = n / buffer_size() + 1;
    
    // 一个deque最小管理8个，最多node_count + 2，前后各预留一个
    map_size = max(initial_map_size(), node_count + 2);
    map = (map_pointer)map_allocator::allocate(map_size);
    // 保持在最中央
    map_pointer nstart = map + (map_size - node_count)/2;
    map_pointer nfinish = nstart + node_count - 1;
    
    map_pointer cur;
    for (cur = nstart; cur <= nfinish; ++cur) {
        *cur = allocate_node();
    }
    
    first.set_node(nstart);
    last.set_node(nfinish);
    first.cur = first.first;
    last.cur  = last.first + n % buffer_size();
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const T& t) {
    value_type t_copy = t; // FIXME: 为什么要copy？
    reserve_map_at_back(); // 是否需要新分配一个map
    *(last.node + 1) = allocate_node();
    
    // 设置当前node最后一个元素，然后把last移到下一个node
    construct(last.cur, t_copy);
    last.set_node(last.node + 1);
    last.cur = last.first;
    
    //__STL_UNWIND(deallocate_node(*(last.node+1))); // TODO: 应该对应的__STL_TRY？
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const T& t) {
    value_type t_copy = t; // FIXME: 为什么要copy？
    reserve_map_at_front(); // 是否需要新分配一个map
    *(first.node - 1) = allocate_node();
    
    first.set_node(first.node - 1);
    first.cur = first.last - 1;
    construct(first.cur, t_copy);
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type size_to_add, bool is_front) {
    size_type old_node_count = last.node - first.node + 1;
    size_type new_node_count = old_node_count + size_to_add;
    
    map_pointer new_node_start;
    // 如果需要的比较少，但是本身还有很多空间，那么把node居中以下
    if(map_size > 2 * new_node_count) {
        new_node_start = map + (map_size - new_node_count) / 2 + (is_front ? size_to_add : 0);
        if(new_node_start < first.node) {
            copy(first.node, last.node + 1, new_node_start);
        }
        else {
            copy_backward(first.node, last.node+1, new_node_start + old_node_count);
        }
    }
    else { // 需要重新分配一个新的空间
        size_type new_map_size = map_size + max(map_size, size_to_add) + 2;
        map_pointer new_map = (map_pointer)map_allocator::allocate(new_map_size);
        new_node_start = new_map + (new_map_size - new_node_count) / 2 + (is_front ? size_to_add : 0);
        copy(first.node, last.node + 1, new_node_start);
        deallocate_node(map, map_size);
        
        map = new_map;
        map_size = new_map_size;
    }
    
    first.set_node(new_node_start);
    last.set_node(new_node_start + old_node_count - 1);
}

END_MINISTL


