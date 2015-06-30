#pragma once

#include "common.h"

START_MINISTL

////////////////// COMPARISONS ////////////////////////
template<class T>
struct greater {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs > rhs;
	}
};

template<class T>
struct greater_equal {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs >= rhs;
	}
};

template<class T>
struct less {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs < rhs;
	}
};

template<class T>
struct less_equal {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs <= rhs;
	}
};

template<class T>
struct equal_to {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs == rhs;
	}
};

template<class T>
struct not_equal_to {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs != rhs;
	}
};

////////////////// LOGICAL OPS ////////////////////////
template<class T>
struct logical_and {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs && rhs;
	}
};

template<class T>
struct logical_or {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs || rhs;
	}
};

template<class T>
struct logical_not {
	bool operator()(const T& arg) const {
		return !arg;
	}
};

////////////////// BIT OPERATIONS ////////////////////////
template<class T>
struct bit_and {
	T operator()(const T& lhs, const T& rhs) const {
		return lhs & rhs;
	}
};

template<class T>
struct bit_or {
	T operator()(const T& lhs, const T& rhs) const {
		return lhs | rhs;
	}
};

template<class T>
struct bit_xor {
	T operator()(const T& lhs, const T& rhs) const {
		return lhs ^ rhs;
	}
};


END_MINISTL