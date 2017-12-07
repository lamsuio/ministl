#pragma once

#include "memory.h"
#include "iterator.h"
#include "functional.h"

START_MINISTL

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
    // 5 types of interator
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
        desc();
        return *this;
    }

    self operator--(int) { // REMARK: return value type
        self tmp = *this;
        desc();
        return tmp;
    }

	bool operator==(const rb_tree_iterator& other) const {
		return this->node == other.node;
	}

	bool operator!=(const rb_tree_iterator& other) const {
		return this->node != other.node;
	}

    // next element of node
    void incr() {
        if (node->right_ != nullptr) {
            node = node->right_;
            while (node->left_)
                node = node->left_;
        } else {
            link_type p = node->parent_;
            while (node == p->right_) {
                node = p;
                p = node->parent_;
            }
            // header's right point to most right
            // when only root, pre while loop will make `node == header`, and
            // `p == root`, that what we need, so we need exclude such condition
            if(node->right_ != p)
                node = p;
        }
    }

    // previous element of node
    void desc() {
        if (node->color_ == kRed &&
            node->parent_->parent_ == node)
            node = node->right_;
        else if (node->left_ != 0) {
            link_type y = node->left_;
            while (y->right_){
                y = y->right_;
            }
            node = y;
        } else {
            link_type y = node->parent_;
            while (node == y->left_) {
                node = y;
                y = y->parent_;
            }
            node = y;
        }
    }
};

template<class R, class Arg>
struct self {
    R& operator()(const Arg& v) const {
        return (R&)v;
    }
};

// For test rb tree properties, friend of rb_tree to access private/protected
// members. Only test purpose.
template <class>
struct __rb_test_helper;

template<class Key,
         class Value = Key,
         class Compare= less<Key>,
         class KeyOfValue=self<Key, Value>,
         class Alloc = alloc>
class rb_tree {
protected:
    typedef void* void_pointer;
public:
    typedef Key                 key_type;
    typedef Value               value_type;
    typedef size_t              size_type;
    typedef rb_tree_color       color_type;
    typedef rb_tree_node<Value>*  link_type;
	typedef rb_tree_node<Value>	  node_type;
    typedef value_type&         reference;
    typedef value_type*         pointer;
protected:
	typedef mini_alloc<node_type, Alloc> node_alloc;

    // Wrapper of allocate and dealloacte
	link_type get_node() { return (link_type)node_alloc::allocate(); }
	void put_node(link_type p) { node_alloc::deallocate(p); }

    link_type create_node(const value_type& x) {
        link_type tmp = get_node();
		construct(&tmp->value_, x); // Some exception not captured
        return tmp;
    }

    void destroy_node(link_type node) { destroy(&node->value_); put_node(node); }
    void destroy_subnode(link_type node) {
        if (node->left_ != nullptr)
            destroy_subnode(node->left_);
        if (node->right_ != nullptr)
            destroy_subnode(node->right_);
        destroy_node(node);
    }

    // Copy value and color only
    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value_);
        tmp->color_ = x->color_;
        tmp->left_  = nullptr;
        tmp->right_ = nullptr;
        tmp->parent_ = nullptr;
        return tmp;
    }

protected:
    // header, parent of root，which parent is root and left is the most-left
    // node, right is the most-right node.
    link_type  header;
    size_type  node_count;
    Compare    key_compare;
    KeyOfValue key_of_value;

    // quick helper function to fetch root/leftmost/rightmost
    link_type& root() { return header->parent_; }
    link_type& leftmost() { return header->left_; }
    link_type& rightmost() { return header->right_; }

    // member of node
    static link_type&  left(link_type x)     { return x->left_; }
    static link_type&  right(link_type x)    { return x->right_; }
    static link_type&  parent(link_type x)   { return x->parent_; }
    static reference   value(link_type x)    { return x->value_; }
	static color_type& color(link_type x)    { return x->color_; }
	const Key& key(link_type x) { return KeyOfValue()(x->value_); } //TODO:

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
    // iterator type
    typedef rb_tree_iterator<value_type, reference, pointer> iterator;

	void clear();

private:
	iterator  header_iterator;

    void copy_node(link_type x, link_type y);
    void swap_node(link_type x, link_type y);
    void erase(link_type x);

    void init() {
        header = get_node();
		header_iterator = iterator(header);
        color(header) = kRed;
        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
		node_count = 0;
    }

    // replace or insert a provided value into the tree
    iterator do_insert_equal(const value_type& v) {
        link_type y = header;
        link_type x = root();
        while (x) {
            y = x; // remember x's parent
            x = key_compare(v, key(x)) ? left(x) : right(x);
        }
        return do_insert(x, y, v);
    }

    // insert with flag node, parent node and value
    iterator do_insert(link_type x, link_type y, const value_type& v) {
        link_type z = create_node(v);
        if (y == header || x || key_compare(v, key(y))) {
            left(y) = z;
            if (y == header) {
                root() = z; // first node
                leftmost() = z;
                rightmost() = z;
            } else if (y == leftmost()) {
                leftmost() = z;
            }
        } else {
            right(y) = z;
            if (y == rightmost())
                rightmost() = z;
        }
        parent(z) = y;
        left(z)   = nullptr;
        right(z)  = nullptr;
        rb_rebalance(z, header->parent_); // root
        ++node_count;
        return iterator(z);
    }

    void do_remove(const value_type& v);

    iterator do_search(const value_type& v) const {
        link_type node = root();
        while (node != nullptr) {
            if (equal_to<Key>(v, key(node))) {
                return iterator(node);
            }
            node = key_compare(v, key(node)) ? left(node) : right(node);
        }
        return end();
    }

	void rb_rebalance(link_type x, link_type& root);

	void rb_tree_rotate_left(link_type x, link_type& root); // counter-clockwise, left rotation

	void rb_tree_rotate_right(link_type x, link_type& root); // clockwise, right rotation

    void rb_tree_rotate(bool is_left, link_type x, link_type& root) {
        is_left ? rb_tree_rotate_left(x, root) : rb_tree_rotate_right(x, root);
    }

    // a test inject to access tree's inner structure
    friend __rb_test_helper<rb_tree>;

public:
	rb_tree(){ init(); }

	~rb_tree(){
		clear();
		destroy_node(header);
	}

	iterator  begin()   const { return iterator(header->left_); }
	iterator  end()     const { return header_iterator; }
	size_type size()    const { return node_count; }

    void insert(const value_type& value) { do_insert_equal(value);}
    void remove(const value_type& value) { do_remove(value);}
    iterator  search(const value_type& value) const { return do_search(value);}
};

template<class Key, class Value, class KeyCompare, class KeyOfValue, class Alloc>
void rb_tree<Key, Value, KeyCompare, KeyOfValue, Alloc>::copy_node(link_type d, link_type s) {
    left  (d) = left  (s);
    right (d) = right (s);
    color (d) = color (s);
    parent(d) = parent(s);

    // replace all link to s with d
    if (s == left(parent(s))) {
        left(parent(d)) = d;
    } else {
        right(parent(d)) = d;
    }

    if (leftmost() == s) {
        leftmost() == d;
    }

    if (rightmost() == s) {
        rightmost() == d;
    }

    if (root() == s) {
        root() == d;
    }
}

template<class Key, class Value, class KeyCompare, class KeyOfValue, class Alloc>
void rb_tree<Key, Value, KeyCompare, KeyOfValue, Alloc>::swap_node(link_type x, link_type y) {
    node_type tmp;

    if (x == header || y == header)
        return;

    copy_node(&tmp, x);
    copy_node(x, y);
    copy_node(y, &tmp);
}

template<class Key, class Value, class KeyCompare, class KeyOfValue, class Alloc>
void rb_tree<Key, Value, KeyCompare, KeyOfValue, Alloc>::clear(){
    link_type node = root();
    if (node != nullptr) {
        destroy_subnode(node);
    }
    root() = nullptr;
    leftmost() = header;
    rightmost() = header;
	node_count = 0;
}

template<class Key, class Value, class KeyCompare, class KeyOfValue, class Alloc>
void rb_tree<Key, Value, KeyCompare, KeyOfValue, Alloc>::do_remove(const value_type& v) {
    iterator it = do_search(it);
    if (it == end()) {
        return;
    }

    link_type node = it.node;
    if (left(node) != nullptr && right(node) != nullptr) {
        ++it;
        link_type prev = it.node;
        swap_node(prev, node);
    }

    link_type child = left(node) == nullptr ? right(node) : left(node);

    if (child != nullptr) {
        // the node has one child, the child must be red and node must be black.
        // so simply swap node and child, and delete node directly.
        swap_node(child, node);
    } 

    --node_count;

    // fix header's ptr
    if (leftmost() == node) {
        leftmost() = parent(node);
    }
    if (rightmost() == node) {
        rightmost() = parent(node);
    }
    if (root() == node) {
        // 1. the node is root
        root() = nullptr;
    } else {
        bool is_left_child = false;
        link_type p = parent(node);
        link_type bro = left(p);
        if (node == left(p)) {
            left(p) = nullptr;
            bro = right(p);
            is_left_child = true;
        } else {
            right(p) = nullptr;
        }

        do {
            // 2. no bro(single child)
            if (bro == nullptr) {
                break;
            }
            // 3. has bro, and node is red, nothing changed
            if (color(node) == kRed) {
                break;
            }

            // node is black, make imbalance
            color(bro) = color(p);  // copy parent color
            color(p) = kBlack;      // make parent black, keep black number
            if (left(bro) == nullptr && right(bro) == nullptr) {
                if (color(bro) == kRed) {
                    // balance now
                    break;
                } else {
                    // we need reduce black number!!!

                }
            } else if (right(bro) == nullptr) {
                // bro has left child, so we rotate to one line
                if (is_left_child) {
                    rb_tree_rotate(!is_left_child, bro, root())
                }
            } else if (left(bro) == nullptr) {
                if (!is_left_child) {
                    rb_tree_rotate(is_left_child, bro, root())
                }
            } 
            if (is_left_child) {
                color(right(bro)) = kBlack;
            } else {
                color(left(bro)) = kBlack;
            }

            // parent is red, and bro has two child with red(MUST BE)
            rb_tree_rotate(is_left_child, p, root());
        } while (false);
    }

}

template<class Key, class Value, class KeyCompare, class KeyOfValue, class Alloc>
void rb_tree<Key, Value, KeyCompare, KeyOfValue, Alloc>::rb_rebalance(link_type x, link_type& root){
	x->color_ = kRed;
	while (x != root && x->parent_->color_ == kRed) {   // parent is RED
		if (x->parent_ == x->parent_->parent_->left_) { // p-p's left son, so check right uncle
			link_type uncle = x->parent_->parent_->right_;
			if (uncle && uncle->color_ == kRed) {       // Uncle is RED
                /**
                 *      pp?        (x)  <- recheck from pp
                 *     /  \        / \
                 *   (p)  (u) --> p   u  <- turn uncle/parent to black
                 *    |           |
                 *   (x)         (_)
                 */
				x->parent_->color_ = kBlack;
				uncle->color_ = kBlack;
				x->parent_->parent_->color_ = kRed;
				x = x->parent_->parent_;
			} else { // No uncle or uncle is BLACK
				if (x == x->parent_->right_) {
                    /**
                     * if x is right son, then rotate to become left son
                     *
                     *      pp?           pp?             pp?
                     *     /  \          /  \            /  \
                     *   (p)  u?  -->  (x)   u? --l--> (_)   u?
                     *   / \           / \             / \
                     *     (x)           (_)         (x)
                     */
					x = x->parent_;
					rb_tree_rotate_left(x, root);
				}
                /**
                 * x now is left son, then change p & pp color and right rotate
                 *      pp?         (pp)  --r-->  p
                 *     /  \         /  \         / \
                 *   (p)   u? -->  p    u?     (x) (pp)
                 *   / \          / \              /  \
                 * (x)  m       (x)  m            m    u?
                 */
				x->parent_->color_ = kBlack;
				x->parent_->parent_->color_ = kRed;
				rb_tree_rotate_right(x->parent_->parent_, root);
			}
		} else { // p is parent's right son
			link_type uncle = x->parent_->parent_->left_;
			if (uncle && uncle->color_ == kRed) {
                /**
                 *      pp?        (x)  <- recheck from pp
                 *     /  \        / \
                 *   (u)  (p) --> u   p  <- turn uncle/parent to black
                 *         |          |
                 *        (x)        (_)
                 */
				x->parent_->color_ = kBlack;
				uncle->color_ = kBlack;
				x->parent_->parent_->color_ = kRed;
				x = x->parent_->parent_;
			} else {
				if (x == x->parent_->left_) {
                   /**
                    * We want process right son only, rotate if left son
                    *
                    *      pp?         pp?           pp?
                    *     /  \        / \           /  \
                    *    u?  (p) --> u? (x) --r--> u?  (_)
                    *        / \        / \            / \
                    *      (x)  m     (_)  m          A  (x)
                    *                 / \                /  \
                    *                A   B              B    m
                    */
					x = x->parent_;
					rb_tree_rotate_right(x, root);
				}
                /**
                 *      pp?        (pp)  --l-->   p
                 *     /  \        /  \          / \
                 *    u?  (p) --> u?   p       (pp) (x)
                 *        / \         / \      / \
                 *       m  (x)      m  (x)   u?  m
                 */
				x->parent_->color_ = kBlack;
				x->parent_->parent_->color_ = kRed;
				rb_tree_rotate_left(x->parent_->parent_, root);
			}
		}
	}// End While
	root->color_ = kBlack;
}

// Left Rotation
/**
 *   X           Y
 *  / \    =>   / \
 *     Y       X
 *
 **/
template<class Key, class Value, class KeyCompare, class KeyOfValue, class Alloc>
void rb_tree<Key, Value, KeyCompare, KeyOfValue, Alloc>::rb_tree_rotate_left(link_type x, link_type& root){
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

// Right Rotation
/**
 *    (X)         (Y)
 *    / \    =>   / \
 *  (Y)  C       A  (X)
 *  / \             / \
 * A   B           B   C
 **/
template<class Key, class Value, class KeyCompare, class KeyOfValue, class Alloc>
void rb_tree<Key, Value, KeyCompare, KeyOfValue, Alloc>::rb_tree_rotate_right(link_type x, link_type& root) {
	link_type y = x->left_; // target's left
	x->left_ = y->right_;
	if (y->right_)
		y->right_->parent_ = x;
	y->parent_ = x->parent_;

	if (x == root)
		root = y;
	else if (x == x->parent_->right_) // Lose a equal assertion
		x->parent_->right_ = y;
	else
		x->parent_->left_ = y;
	y->right_ = x;
	x->parent_ = y;
}

//
// 1. node is red or black(y)
// 2. root is black(y)
// 3. all leaf is black(y)
// 4. every red node has two black children
// 5. every path from root to any leaf, must contains the same number of the black node
//
// so we should verify 4&5 prperties
template <class rb_tree>
struct __rb_test_helper {
    typedef typename rb_tree::iterator   iterator;
    typedef typename rb_tree::link_type  link_type;

    const rb_tree* tree;

    __rb_test_helper(const rb_tree* t) { tree = t; }

    link_type root() {
        // the tree->root() return ref
        return tree->header->parent_;
    }

    iterator next_edge_node(iterator it) {
        for (++it; it != tree->end(); ++it) {
            link_type x = it.node;
            // if left is empty, that means we approached one edge now.
            // because the cursor is forward, so we need check left firstly.
            if (rb_tree::left(x) == nullptr) {
                return it;
            } else if (rb_tree::right(x) == nullptr) {
                // if right is empty, that means another edge approached.
                return it;
            }
        }
        return it;
    }

    int black_count(iterator it) {
        link_type x = it.node;
        int blackCount = 0;
        while (x != tree->header->parent_) {
            if (rb_tree::color(x) == kBlack) {
                ++blackCount;
            }
            x = rb_tree::parent(x);
        }

        return blackCount;
    }

    // 1. node is red or black
    // return end if fit, or return illegal iterator
    iterator rule_1() {
        for (auto it = tree->begin(); it != tree->end(); ++it) {
            link_type x = it.node;
            if (rb_tree::color(x) != kRed && rb_tree::color(x) != kBlack) {
                return it;
            }
        }
        return tree->end();
    }

    // 2. root is black
    iterator rule_2() {
        return rb_tree::color(root()) == kBlack ? tree->end() : iterator(root());
    }

    // 3. all leaf is black(y)
    iterator rule_3() {
        return tree->end();
    }

    // 4. every red node has two black children
    // return end if fit, or return illegal iterator
    iterator rule_4() {
        for (auto it = tree->begin(); it != tree->end(); ++it) {
            link_type x = it.node;
            if (rb_tree::color(x) == kRed) {
                link_type l = rb_tree::left(x);
                link_type r = rb_tree::right(x);
                if ((l != nullptr && rb_tree::color(l) == kRed) ||
                    (r != nullptr && rb_tree::color(r) == kRed)) {
                    return it;
                }
            }
        }
        return tree->end();
    }

    // 5. every path from root to any leaf, must contains the same number of the black node
    // return end if fit, or return illegal iterator(edge node, contains most one child)
    iterator rule_5() {
        iterator it = tree->begin();
        int blackCount = black_count(it);
        while (it != tree->end()) {
            if (black_count(it) != blackCount) {
                break;
            }
            it = next_edge_node(it);
        }
        return it;
    }
};

END_MINISTL
