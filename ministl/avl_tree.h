#pragma once

#include "memory.h"

START_MINISTL

//
// AVL Tree Node
//
template <class T>
struct avl_tree_node {
    struct avl_tree_node<T> *left;
    struct avl_tree_node<T> *right;
    struct avl_tree_node<T> *parent;
    T       value;
    size_t  height;
};

template<typename T>
void update_node(avl_tree_node<T>* node){
    if (node != NULL) {
        size_t lh, rh;
        lh = node->left == NULL ? 0: node->left->height;
        rh = node->right == NULL ? 0: node->right->height;
        node->height = lh > rh ? (lh + 1) : (rh + 1);
    }
}

template<typename T>
bool is_balance_node(avl_tree_node<T>* node) {
    int dis;
    dis = (node->left == NULL ? 0: node->left->height);
    dis -= (node->right == NULL ? 0: node->right->height);
    return dis < 2 && dis > -2;
}

template<typename T>
size_t get_node_height(avl_tree_node<T>* node) {
    if (node != nullptr) return node->height;
    return 0;
}

template<class T, class Pointer, class Reference>
class avl_tree_iterator {
public:
    typedef avl_tree_iterator<T, Pointer, Reference> self;
    typedef T           value_type;
    typedef Pointer     pointer;
    typedef Reference   reference;
    typedef size_t      size_type;
    //typedef bidirection_iterator_tag category;
    typedef ptrdiff_t   difference;
protected:
    typedef avl_tree_node<T>* node_type;

public:
    avl_tree_iterator(): node(0) {}
    avl_tree_iterator(const node_type n): node(n) {}
    avl_tree_iterator(const self& other): node(other.node) {}
    self& operator=(const self& other) {this->node = other.node; return *this; }

    value_type& operator*() { return node->value;}
    pointer operator->() { return &(*this);}

    // Self increment
    self& operator++() {
        node_type next = node->right;
        if(next) {
            // the next in the right-sub-tree
            while(next->left){
                next = next->left;
            }
            node = next;
        }
        else {
            // the next is in the subtree of parent
            node_type p = node->parent;
            while (p != nullptr && node == p->right) {
                node = p;
                p = node->parent;
            }
            node = p;
        }

        return *this;
    }

    self operator++(int){
        self tmp = *this;
        ++(*this);
        return tmp;
    }

    self& operator--(){
        node_type next = node->left;
        if (next != nullptr) {
            // previous is in the subtree of node
            while(next->right) {
                next = next->right;
            }
            node = next;
        }
        else {
            node_type p = node->parent;
            while (p != nullptr && node == p->left) {
                node = p;
                p = node->parent;
            }
            node = p;
        }
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator==(const self& other) {
        return other.node == this->node;
    }

    bool operator!=(const self& other) {
        return !operator==(other);
    }
protected:
    node_type node;
};

template<class T, class Alloc = alloc>
class avl_tree {
public:
    typedef avl_tree_iterator<T, T*, T&>    iterator;
protected:
    typedef struct avl_tree_node<T>* node_type;
    typedef mini_alloc<avl_tree_node<T>, Alloc>     node_allocator;
    node_type root, header;
    iterator  terminator; // the terminator

    node_type get_node() { return (node_type)node_allocator::allocate(); }
    void put_node(node_type node) { node_allocator::deallocate(node); }

    node_type create_node(const T& x) {
        node_type node = get_node();
        construct(&node->value, x);
        node->left = node->right = node->parent = NULL;
        node->height = 1;
        return node;
    }

    void destroy_node(node_type node) { destroy(&node->value); put_node(node); }
    void destroy_subnode(node_type node) {
        if (node->left != nullptr) 
            destroy_subnode(node->left);
        if (node->right != nullptr)
            destroy_subnode(node->right);
        destroy_node(node);
    }
    void empty_initialize() {
        header = create_node(T());
        root = nullptr;
        terminator = iterator(nullptr);
    }

private:
    void rotate_closewise(node_type top);
    void rotate_unclosewise(node_type top);
    void ll_rotation(node_type& top);
    void lr_rotation(node_type& top);
    void rl_rotation(node_type& top);
    void rr_rotation(node_type& top);
    void rebalance(node_type& top);
    node_type& insert(node_type& parent, node_type node);
    void remove(node_type& parent, T val);

public:
    // Constructor of class
    avl_tree() { empty_initialize(); }
    virtual ~avl_tree() {
		if (root != nullptr)
			destroy_subnode(root);
        destroy_node(header);
    }
    // TODO: Copy Constructor and operator
    //

    void push(const T& x);
    T& pop();
    node_type top() { return root;}
    iterator first() const {
        node_type cursor = root;
        while (cursor!= nullptr && cursor->left != nullptr) {
            cursor = cursor->left;
        }
        return iterator(cursor);
    }

    iterator end() const { return terminator;}
};

// Internal insert method
template<class T, class Alloc>
typename avl_tree<T,Alloc>::node_type&
avl_tree<T, Alloc>::insert(node_type& parent, node_type node) {
    if (parent == NULL) {
        parent = node;
        return parent;
    }

    if (parent->value <= node->value) {
        insert(parent->right, node)->parent = parent;
    }
    else {
        insert(parent->left, node)->parent = parent;
    }
    update_node(parent);
    if (!is_balance_node(parent)){
        rebalance(parent);
    }
    return parent;
}

template<class T, class Alloc>
void avl_tree<T, Alloc>::push(const T& x) {
    node_type node = create_node(x);
    insert(root, node);
}

template<class T, class Alloc>
void avl_tree<T, Alloc>::rotate_closewise(node_type top) {
    if (top->left == NULL) return;

    node_type lr = top->left->right;
    top->left->right = top;
    top = top->left;
    top->right->left = lr;
    update_node(top->right);
    update_node(top);
}

template<class T, class Alloc>
void avl_tree<T, Alloc>::rotate_unclosewise(node_type top) {
    if (top->right == NULL) return;
}

// Rotate left
//   (X)             A
//   /  \           / \
//  A    B    ==>  C  (X)
// / \                / \
//C  D               D   B
template<class T, class Alloc>
void avl_tree<T, Alloc>::ll_rotation(node_type& top) {
    node_type lr = top->left->right;    // Save D
    node_type parent = top->parent;     // Save top parent
    top->left->right = top;             // set A.right to X
    top->parent = top->left;
    top = top->left;                    // set top to A
    top->parent = parent;
    top->right->left = lr;              // set x.left = D
    if (lr != nullptr) lr->parent = top->right;      // fix D.parent
    update_node(top->right);
    update_node(top);
}

// Rotate left then right(or reverse?)
//   (X)           (X)
//   / \   (A_r_D) / \   (D_l_X)
// [A]  B    ==> [D]  B  ==>  ...
// / \           / \
//C [D]        [A]  F
//  / \        / \
// E   F      C  E
template<class T, class Alloc>
void avl_tree<T, Alloc>::lr_rotation(node_type& top) {
    //node_type lrl = top->left->right->left; // save E
    //top->left->right->left = top->left;     // set D.left = A
    //top->left = top->left->right;           // set X.left = D
    //top->left->left->right = lrl;           // set A.right = E
    //update_node(top->left->left);           // fix A height
    //update_node(top->left);                 // fix D height
    rr_rotation(top->left);
    ll_rotation(top);
}

// Rotate right
//    (X)            B
//    / \           / \
//   A   B    ==> (X)  D
//      / \       / \
//     C   D     A   C
template<class T, class Alloc>
void avl_tree<T, Alloc>::rr_rotation(node_type& top) {
    node_type rl = top->right->left; // C
    node_type parent = top->parent;
    top->right->left = top;          // set B left to X
    top->parent = top->right;
    top = top->right;                // set Top to B,
    top->parent = parent;
    top->left->right = rl;           // now top is B, fix X.right
    if (rl != nullptr) rl->parent = top->left;
    update_node(top->left);          // fix X.height
    update_node(top);                // fix B.height
}

// Rotate right then left(or reverse?)
//   (X)           (X)
//   / \   (C_l_B) / \   (C_r_X)
//  A   B    ==>  A   C  ==>  ...
//     / \           / \
//    C   D         E   B
//   / \               / \
//  E   F             F   D
template<class T, class Alloc>
void avl_tree<T, Alloc>::rl_rotation(node_type& top) {
    if (top->right == NULL)
        return;
    //node_type rlr = top->right->left->right;    // save F
    //top->right->left->right = top->right;       // set C.right = B
    //top->right = top->right->left;              // set X.right = C
    //top->right->right->left = rlr;              // set B.left = F
    //update_node(top->right->right);             // fix B height
    //update_node(top->right);                    // fix C height
    ll_rotation(top->right);
    rr_rotation(top);
}

template<class T, class Alloc>
void avl_tree<T, Alloc>::rebalance(node_type& top) {
    if(get_node_height(top->left) > get_node_height(top->right)) {
        node_type left = top->left;
        if (left!= NULL && get_node_height(left->left) > get_node_height(left->right)) {
            return ll_rotation(top);
        }
        else {
            return lr_rotation(top);
        }
    }
    else {
        node_type right = top->right;
        if (right != NULL) {
            if (get_node_height(right->left) > get_node_height(right->right)) {
                return rl_rotation(top);
            }
            else {
                return rr_rotation(top);
            }
        }
    }
}

template<class T, class Alloc>
void avl_tree<T, Alloc>::remove(node_type& parent, T value) {
    if (parent == NULL) return;

    if (value > parent->value)
        remove(parent->right, value);
    else if (value < parent->value)
        remove(parent->left, value);
    else {
        if (parent->left && parent->right) {
            // get the next one of current node
            node_type tmp = parent->right;
            while(tmp) {
                tmp = tmp->left;
            }
            parent->value = tmp->value;
            tmp->value = value;
            remove(parent->right, value);
        }
    }
}

END_MINISTL
