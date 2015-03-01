#pragma once

START_MINISTL

template<typename T, typename U>
T static_cast(U& u) {
    return (T)u;
}

template<typename T>
T&& forward(T& t) {
    return static_cast<T&&>(t);
}

END_MINISTL
