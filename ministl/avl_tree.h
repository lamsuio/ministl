#pragma once

#include "memory.h"

START_MINISTL

template <class T>
struct avl_tree_node {
    struct avl_tree_node<T> *left;
    struct avl_tree_node<T> *right;
    struct avl_tree_node<T> *parent;
    T value;
};

template<class T, class Alloc = alloc>
class avl_tree {
    protected:
        typedef struct avl_tree_node<T>* node_type;
        typedef mini_alloc<T, Alloc>     node_allocator;
        node_type root;

        node_type get_node() { return (node_type)node_allocator::allocate(); }
        void put_node(node_type node) { node_allocator::deallocate(node); }

        node_type create_node(const T& x) {
            node_type node = get_node();
            construct(&node->value, x);
            node->left = node->right = node->parent = NULL;
            return node;
        }

        void destroy_node(node_type node) { destroy(&node->value); put_node(node); }
        void empty_initialize() { root = NULL; }

    private:
        void rotate_closewise(node_type top);
        void rotate_unclosewise(node_type top);

    public:
        avl_tree() { empty_initialize(); }
        void push(const T& x);
        T& pop();
        node_type top() { return root;}
};

template<class T, class Alloc>
void avl_tree<T, Alloc>::push(const T& x) {
    node_type node = create_node(x);
    if( root == NULL) {
        root = node;
    }
    else {
        node_type tmp = root;
        bool t1 = false;
        bool t2 = false;

        while (tmp != NULL) {
            t1 = t2;
            t2 = false;
            if (tmp->value > node->value) {
                t2 = true;
                if (tmp->left == NULL) {
                    tmp->left = node;
                    node->parent = tmp;
                    break;
                }
                tmp = tmp->left;
            }
            else {
                if (tmp->right == NULL) {
                    tmp->right = node;
                    node->parent = tmp;
                    break;
                }
                tmp = tmp->right;
            }
        }

        // LL
        if (t1 && t2) {
            if (tmp->parent && tmp->parent->parent)
                rotate_closewise(tmp->parent->parent);
            else if (tmp->parent)
                rotate_closewise(tmp->parent);
        }
        // RR
        if (t1 && t2) {
            if (tmp->parent && tmp->parent->parent)
                rotate_unclosewise(tmp->parent->parent);
            else if (tmp->parent)
                rotate_unclosewise(tmp->parent);
        }

        // RL
        if (!t1 && t2) {
            if (tmp->parent){
                rotate_unclosewise(tmp->parent);
                if (tmp->parent->parent)
                    rotate_closewise(tmp->parent->parent);
            }
        }

        // LR
        if (t1 && !t2) {
            if (tmp->parent){
                rotate_closewise(tmp->parent);
                if (tmp->parent->parent)
                    rotate_unclosewise(tmp->parent->parent);
            }
        }
    }
}

template<class T, class Alloc>
void avl_tree<T, Alloc>::rotate_closewise(node_type top) {
    if (top->left == NULL) return;

    node_type succ = top->left;
    node_type parent = top->parent;

    top->left = succ->right;
    top->left->parent = top;

    succ->right = top;
    top->parent = succ;

    succ->parent = parent;
    if (parent != NULL) {
        if (parent->left == top) parent->left = succ;
        parent->right = succ;
    }

}

template<class T, class Alloc>
void avl_tree<T, Alloc>::rotate_unclosewise(node_type top) {
    if (top->right == NULL) return;

    node_type succ = top->right;
    node_type parent = top->parent;

    top->right = succ->left;
    top->right->parent = top;

    succ->left = top;
    top->parent = succ;

    succ->parent = parent;
    if (parent != NULL) {
        if( parent->left == top ) parent->left = succ;
        else parent->right = succ;
    }
}

END_MINISTL
