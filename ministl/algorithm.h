#pragma once

#include "iterator.h"
#include "memory.h"

START_MINISTL

template<class T>
const T& max(const T& left, const T& right) {
    return (left > right) ? left : right;
}

template<class T>
T& min(const T&& left, const T&& right) {
    return (left < right) ? left : right;
}

END_MINISTL
