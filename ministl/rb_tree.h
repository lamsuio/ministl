#pragma once

#include "memory.h"
#include "iterator.h"

enum rb_tree_color {kRed, kBlack};

template<class T>
struct rb_tree_node
{
    T value_;
    rb_tree_color  color_;
    rb_tree_node<T>*  parent_;
    rb_tree_node<T>*  left_;
    rb_tree_node<T>*  right_;
};

template<class T, class Reference, class Pointer>
struct rb_tree_iterator
{
    typedef rb_tree_node<T>*                        link_type;
    typedef rb_tree_iterator<T, Reference, Pointer> self;
    typedef rb_tree_iterator<T, T&, T*>             iterator;
    typedef rb_tree_iterator<T, const T&, const T*> const_iterator;
    // 迭代器5类型
    typedef ptrdiff_t   difference_type;
    typedef T           value_type;
    typedef Reference   reference;
    typedef Pointer     pointer;
    typedef bidirectional_iterator_tag  iterator_category;
    
    link_type node;
    
    rb_tree_iterator() {}
    rb_tree_iterator(link_type x) { node = x; }
    rb_tree_iterator(const iterator& it) { node = it.node; }
    
    reference operator*() const { return node->value_; }
    pointer operator->() const { return &(operator*()); }
    
    self& operator++() {
        incr();
        return *this;
    }
    
    self operator++(int) {
        self tmp = *this;
        incr();
        return tmp;
    }
    
    self& operator--() {
        incr();
        return *this;
    }
    
    self operator--(int) { // 注意，这里和上面的返回不一样
        self tmp = *this;
        desc();
        return tmp;
    }
    
    //前进一位
    void incr() {
        if (node->right_ != nullptr) {
            node = node->right_;
            while (node->left_)
                node = node->left_;
        }
        else {
            link_type p = node->parent_;
            while (node == p->right_) {
                node = p;
                p = p->parent_;
            }
            if(node->right_ != p) // 对应rb的特殊构造
                node = p;
        }
        
    }

    void desc() 
    {
        if (node->color_ == kRed &&
            node->parent_->parent_ == node)
            node = node->right_; // 对应rb的特殊构造
        else if (node->left_ != 0) {
            link_type y = node->left_;
            while (y->right_){
                y = y->right_;
            }
            node = y;
        }
        else {
            link_type y = node->parent_;
            while (node == y->left_) {
                node = y;
                y = y->parent_;
            }
            node = y;
        }
    }
};

template<class Key, class Value, class Compare, class KeyOfValue, class Alloc = alloc>
class rb_tree {
protected:
    typedef void* void_pointer;
public:
    // 其他的 typedef
    typedef Key                 key_type;
    typedef Value               value_type;
    typedef size_t              size_type;
    typedef rb_tree_color       color_type;
    typedef rb_tree_node<Value>*  link_type;
    typedef value_type&         reference;
    typedef value_type*         pointer;
protected:
    typedef mini_alloc<rb_tree_node<Value>, Alloc> node_alloc;

    // 创建和销毁的包装
    link_type get_node() { return (link_type)node_alloc::allocate(); }
    void put_node(link_type p) { node_alloc::deallocate(p); }
    
    link_type create_node(const value_type& x) {
        link_type tmp = get_node();
        return tmp;
    }
    
    void destroy_node(link_type node) {
        put_node(node);
    }
    
    // 只复制值和颜色
    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value_);
        tmp->color_ = x->color_;
        tmp->left_  = nullptr;
        tmp->right_ = nullptr;
        tmp->parent_ = nullptr;
        return tmp;
    }
 
protected:
    size_type node_count;//节点数量
    link_type header;    // 头，在root之上，其parent指向root，left指向最左，right指向最右
    Compare   key_compare;  // 比较key的 function object
    KeyOfValue key_of_value;
    
    // 所以会有以下三个方法：
    link_type& root() { return header->parent_; }
    link_type& leftmost() { return header->left_; }
    link_type& rightmost() { return header->right_; }
    
    // 节点成员
    static link_type& left(link_type x) { return x->left_;}
    static link_type& right(link_type x) { return x->right_;}
    static link_type& parent(link_type x) { return x->parent_;}
    static reference  value(link_type x) { return x->value_;}
    static const Key& key(link_type x) { return key_of_value(x->value_);} //TODO:
    static color_type& color(link_type x) { return x->color_;}
    
    // 求极值
    static link_type minimum(link_type x) {
        while(x->left_)
            x = x->left_;
        return x;
    }
    static link_type maximum(link_type x) {
        while(x->right_)
            x = x->right_;
        return x;
    }
    
public:
    // 迭代器类型在此
    typedef rb_tree_iterator<value_type, reference, pointer> iterator;
    
private:
    link_type copy(link_type x, link_type y);
    void erase(link_type x);
    
    void init() {
        header = get_node();
        color(header) = kRed;
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }
    
    iterator insert_equal(const reference v) {
        link_type y = header;
        link_type x = root();
        while (x) {
            y = x; // 记录x的父亲而已
            x = key_compare(key(v), key(x)) ? left(x) : right(x);
        }
        return insert(x, y, v);
    }
    
    iterator insert(link_type x, link_type y, const value_type& v) {
        link_type z = create_node();
        if (y == header || x || key_compare(key(v), key(y))) {
            left(y) = z;
            if (y == header) {
                root() = z; // 第一个点
                rightmost() = z;
            }
            else if (y == leftmost()) {
                leftmost() = z;
            }
        }
        else {
            right(y) = z;
            if (y == rightmost())
                rightmost() = z;
        }
        parent(z) = y;
        left(z)   = nullptr;
        right(z)  = nullptr;
        rb_rebalance(z, header->parent); // 新增， root
        ++node_count;
        return iterator(z);
    }
    
    void rb_rebalance(link_type x, link_type& root) {
        x->color = kRed;
        while (x != root && x->parent_->color_ == kRed) {// 父亲也为红
            if (x->parent_ == x->parent_->parent_->left_) { // 是爷爷的左儿子，那么看右伯父了
                link_type uncle = x->parent_->parent_->right_;
                if (uncle && uncle->color_ == kRed) { // 伯父是红色
                    x->parent_->color_ = kBlack;
                    uncle->color_      = kBlack;
                    x->parent_->parent_->color_ = kRed;
                    x = x->parent_->parent_;
                }
                else { // 不存在伯父，或者伯父是黑色
                    if (x == x->parent_->right_) { // 如果是右儿子
                        x = x->parent_;
                        rb_tree_rotate_left(x, root); // 好了，左旋一下
                    }
                    x->parent_->color_ = kBlack;
                    x->parent_->parent_->color_ = kRed;
                    rb_tree_rotate_right(x->parent_->parent_, root); // 把爷爷右旋一下
                }
            }
            else { // 父亲是爷爷的右儿子
                link_type uncle = x->parent_->parent_->left_;
                if (uncle && uncle->color_ == kRed) {
                    x->parent_->color_ = kBlack;
                    uncle->color_      = kBlack;
                    x->parent_->parent_->color_ = kRed;
                    x = x->parent_->parent_;
                }
                else {// 不存在伯父，或者伯父是黑色
                    if (x == x->parent_->left_) { // 如果是左儿子
                        x = x->parent_;
                        rb_tree_rotate_right(x, root); // 好了，右旋一下
                    }
                    x->parent_->color_ = kBlack;
                    x->parent_->parent_->color_ = kRed;
                    rb_tree_rotate_left(x->parent_->parent_, root); // 把爷爷右旋一下
                }
            }
        }// End While
        root->color = kBlack;
    }
    
    // 左旋
    //   X           Y
    //  / \    =>   / \
    //     Y       X
    //
    void rb_tree_rotate_left(link_type x, link_type& root) {
        link_type y = x->right_; // 旋转点的右儿子
        x->right_ = y->left_;
        if (y->left_) 
            y->left_->parent_ = x;  // 反设
        y->parent_ = x->parent_;
        
        // 让y完全顶替x
        if (x == root) 
            root = y;
        else if (x == x->parent_->left_) // 左儿子
            x->parent_->left_ = y;
        else
            x->parent_->right_ = y;
        y->left_ = x;
        x->parent_ = y;
    }
    
    // 右旋
    //    (X)         (Y)
    //    / \    =>   / \
    //  (Y)  C       A  (X)
    //  / \             / \
    // A   B           B   C
    void rb_tree_rotate_right(link_type x, link_type& root) {
        link_type y = x->left_; // 旋转点的左儿子
        x->left_ = y->right_;
        if (y->right_)
            y->right_->parent_ = x;
        y->parent_ = x->parent_;
        
        if (x == root)
            root = y;
        else if (x->parent_->right_)
            x->parent_->right_ = y;
        else
            x->parent_->left_  = y;
        y->right_ = x;
        x->parent_ = y;
    }
};