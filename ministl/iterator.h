#pragma once

#include <cstddef>
#include "common.h"

START_MINISTL

// Five types of iterator
//

struct input_iterator_tag { };
struct output_iterator_tag { };
struct forward_iterator_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iterator_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };

//
// Root iterator
//

template< class Category,
          class T,
          class Distance  = ptrdiff_t,
          class Reference = T&,
          class Pointer   = T* >
class iterator {
    typedef Category    iterator_category;
    typedef T           value_type;
    typedef Distance    difference_type;
    typedef Reference   reference;
    typedef Pointer     pointer;
};


//
// Iterator traits
//

template<class T>
class iterator_traits {
    typedef typename T::value_type      value_type;
    typedef typename T::difference_type difference_type;
    typedef typename T::reference       reference;
    typedef typename T::pointer         pointer;
    typedef typename T::iterator_category   iterator_category;
};

template<class T>
class iterator_traits<T*> {
    typedef T           value_type;
    typedef ptrdiff_t  difference_type;
    typedef T&          reference;
    typedef T*          pointer;
    typedef random_access_iterator_tag iterator_category;
};

template<class T>
class iterator_traits<const T*> {
    typedef T           value_type;
    typedef ptrdiff_t   difference_type;
    typedef T&          reference;
    typedef T*          pointer;
    typedef random_access_iterator_tag iterator_category;
};

//
// Helper
//

// TODO:maybe use c++11 features
template<class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator& it) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// FIXME: Why use static_const here
template<class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator& it) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

template<class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator& it) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

END_MINISTL
