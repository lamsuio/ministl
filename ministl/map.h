/**
 * Simple implemation of ordered map based on Red-Black Tree.
 */
#pragma once

#include <cstddef>
#include "common.h"
#include "memory.h"
#include "iterator.h"

START_MINISTL

template<class T1, class T2>
struct pair {
    const T1&   first;
    const T2&   second;

    pair(const T1& f, const T2& s):first(f), second(s) {}

    pair(pair&& p) = default;
};

template<class R, template<class R, class> class Pair>
struct key_of_pair {
    R& operator()(const Pair& p) const {
        return (R&)v.first;
    }
};

template<class Key,
         class Value,
         class Compare = less<Key>,
         class Alloc = alloc>
class map : public rb_tree<Key,
                           pair<const Key, Value>,
                           Compare,
                           key_of_pair<Key, Pair<const Key, Value>,
                           Alloc>
{
public:
    typedef rb_tree<Key, pair<const Key, Value>, Compare,
                    key_of_pair<Key, Pair<const Key, Value>, Alloc> base_type;
    typedef typename base_type::iterator                            iterator;

public:
    Value& operator[](const Key& key) {
        iterator it = do_search(key);
    }

    Value& operator[](Key&& key);
};

END_MINISTL
