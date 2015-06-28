#pragma once

#include <new>
#include <cstdlib>
#include <cstring>
#include "common.h"
#include "iterator.h"
#include <cstdio>

START_MINISTL

//
// memory management
//

template<bool threads, int inst>
class __default_alloc_template {
private:
    // structure of memory-link-unit
    union obj {
        union obj * free_list_link;
        char client_data[1];
    };

    enum {__ALIGN = 8}; // We align the memory layout with __ALIGN
    enum {__MAX_BYTES = 128}; // we only handle size less than __MAX_BYTES
    enum {__NFREELISTS = __MAX_BYTES/__ALIGN}; // # of free lists
    enum {__DEFAULT_NUM_CHUNK = 20}; // # of chunk in a node

private:
    static inline size_t ROUND_UP(size_t size) {
        return ((size + __ALIGN - 1 ) & ~(__ALIGN - 1));
    }

private:
    // this memory will leak, so we need another manager to manage
    // the default allocator
    // TODO: not thread-safe
    class __fake_inner_raii
    {
    private:
        void** __inner_mem_to_release;
        size_t __inner_index;
        size_t __inner_size;

    public:
        __fake_inner_raii() {
            __inner_size = __DEFAULT_NUM_CHUNK;
            __inner_mem_to_release = (void **)malloc(__inner_size * sizeof(void*));
            __inner_index = 0;
        }
        void add(void * p) {
            if (__inner_index >= __inner_size){
                void **tmp = __inner_mem_to_release;
                __inner_mem_to_release = (void **)malloc(__inner_size * 2 * sizeof(void *));
                memcpy(__inner_mem_to_release, tmp,  __inner_size * sizeof(void *));
                free(tmp);
                __inner_size *= 2;
            }
            __inner_mem_to_release[__inner_index++] = p;
        }
        ~__fake_inner_raii(){
            for (size_t i = 0; i < __inner_index; i++) {
                free(__inner_mem_to_release[i]);
            }
            free(__inner_mem_to_release);
        }
    };

private:
    static union obj * volatile free_list[__NFREELISTS];

    static inline size_t FREE_INDEX(size_t size) {
        return ((size + __ALIGN - 1)/__ALIGN - 1);
    }

    // Fill the list when no free node in a list.
    // This will be invoked when alloc but free_list is empty.
    // Assume the size is already rounded up to *__ALIGN*
    static void * refill(size_t size) {
        int nobjs = __DEFAULT_NUM_CHUNK;

        char * chunk = chunk_alloc(size, nobjs); // Get chunk from the pool
        obj * volatile * cur_free_list;
        char * result;
        char * current_obj;

        // There's only one room, then take it and keep the free_list empty.
        if (nobjs == 1) return (chunk);

        cur_free_list = free_list + FREE_INDEX(size);
        result = chunk;
        *cur_free_list = (obj*)(result + size); // Set 2nd as head, the 1st is taken.

        // Initialize all node in the list
        for (int i = 1; i < nobjs; i++) {
            current_obj = result + i * size;
            ((obj*)current_obj)->free_list_link = (obj*)(current_obj + size);
        }

        // Set the terminate
        ((obj*)current_obj)->free_list_link = (obj *)NULL;

        return (result);
    }

    // Alloc a chunk memory with n*size. nobjs will be reduce when memory low
    static char * chunk_alloc(size_t size, int& nobjs) {
        char * result;
        size_t need_size = size * nobjs;
        size_t free_size = end_free - start_free;
        static __fake_inner_raii __for_auto_release;

        if (free_size >= need_size) {
            // Pool is big enough
            result = start_free;
            start_free += need_size;
            return (result);
        }
        else if (free_size >= size) {
            nobjs = free_size / size;
            result = start_free;
            start_free += nobjs * size;
            return (result);
        }
        else {
            // Sorry, we could not alloc even ONE
            size_t will_size = need_size * 2 + ROUND_UP(heap_size >> 4);
            if (free_size > 0) {
                obj * volatile * cur_free_list;
                cur_free_list = free_list + FREE_INDEX(free_size);
                ((obj *)start_free)->free_list_link = *cur_free_list;
                *cur_free_list = (obj *)start_free;
            }

            start_free = (char *)malloc(will_size);
            __for_auto_release.add(start_free); // add to release_list
            if (start_free == 0) {
                // There's nothing to be allocated, some crunks in the free_list
                // should be freed. Only consider size > current
                obj * volatile * cur_free_list;
                for (int i = size; i < __MAX_BYTES; i += __ALIGN) {
                    cur_free_list = free_list + FREE_INDEX(i);
                    if (*cur_free_list != 0) {
                        start_free = (char *)*cur_free_list;
                        end_free = start_free + i;
                        *cur_free_list = (*cur_free_list)->free_list_link;

                        // Recurse to fix nobjs
                        return chunk_alloc(size, nobjs);
                    }
                }

                // Sorry
                return (NULL);
            }

            heap_size += will_size;
            end_free = start_free + will_size;
            return chunk_alloc(size, nobjs);
        }
    }

    static char * start_free;
    static char * end_free; // Only be changed in chunk_alloc
    static size_t heap_size;

public:
    static void * allocate(size_t size) {
        obj * volatile * cur_free_list;
        obj * result;

        if (size > __MAX_BYTES){
            return (void *)malloc(size); // FIXME: manage big stuff
        }

        cur_free_list = free_list + FREE_INDEX(size);
        result = *cur_free_list;
        if (result == 0) {
            void * ret = refill(ROUND_UP(size));
            return ret;
        }

        *cur_free_list = result->free_list_link;
        return (void *)(result);
    }

    static void deallocate(void * p, size_t size) {
        obj * value = (obj *)p;
        obj * volatile * cur_free_list;

        if (size > __MAX_BYTES) {
            free(p); //FIXME: manage big stuff
            return;
        }

        cur_free_list = free_list + FREE_INDEX(size);
        value->free_list_link = *cur_free_list;
        *cur_free_list = value;

        return;

    }

    static void * reallocate(void * p, size_t old_size, size_t new_size) {
        void * result;
        size_t should_copy = old_size > new_size ? old_size : new_size;

        deallocate(p, old_size);
        result = allocate(new_size);
        memcpy(p, result, should_copy);

        return result;
    }

};

//
template<bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj * volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = {0};

template<bool threads, int inst>
char * __default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char * __default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;


///////// OBJECT CONSTRUCT //////////////

template<class T1, class T2>
inline void construct(T1 * pointer, const T2& value) {
    new (pointer) T1(value);
}

template<class T>
inline void construct(T * pointer, const T& value) {
    new (pointer) T(value);
}

template<> inline void construct(char * p, const char& v) { *p = v; }
template<> inline void construct(unsigned char * p, const unsigned char& v) {
    *p = v;
}
template<> inline void construct(short * p, const short& v) { *p = v; }
template<> inline void construct(unsigned short * p, const unsigned short& v) {
    *p = v;
}
template<> inline void construct(int * p, const int& v) { *p = v; }
template<> inline void construct(unsigned int * p, const unsigned int& v) {
    *p = v;
}
template<> inline void construct(long * p, const long& v) { *p = v; }
template<> inline void construct(unsigned long * p, const unsigned long& v) {
    *p = v;
}
template<> inline void construct(float * p, const float& v) { *p = v; }
template<> inline void construct(double * p, const double& v) { *p = v; }

template<class T>
inline void destroy(T* pointer) {
    pointer->~T();
}

// destroy a native pointer
template<> inline void destroy(char *) { }
template<> inline void destroy(unsigned char *) { }
template<> inline void destroy(short *) { }
template<> inline void destroy(unsigned short *) { }
template<> inline void destroy(int *) { }
template<> inline void destroy(unsigned int *) { }
template<> inline void destroy(long *) { }
template<> inline void destroy(unsigned long *) { }
template<> inline void destroy(float *) { }
template<> inline void destroy(double *) { }

template<> inline void destroy(char **) { }
template<> inline void destroy(unsigned char **) { }
template<> inline void destroy(short **) { }
template<> inline void destroy(unsigned short **) { }
template<> inline void destroy(int **) { }
template<> inline void destroy(unsigned int **) { }
template<> inline void destroy(long **) { }
template<> inline void destroy(unsigned long **) { }
template<> inline void destroy(float **) { }
template<> inline void destroy(double **) { }

// destroy a serial class poiner
//template<class ForwardIterator, class T>
//inline void destroy(ForwardIterator first, ForwardIterator last) {
//    while(first != last) {
//        (*first)->~T();
//        ++first;
//    }
//}

template<class T>
inline void destroy(T * first, T * last) {
    while(first != last) {
        (*first)->~T();
        ++first;
    }
}
template<> inline void destroy(char *, char *) { }
template<> inline void destroy(unsigned char *, unsigned char *) { }
template<> inline void destroy(short *, short *) { }
template<> inline void destroy(unsigned short *, unsigned short *) { }
template<> inline void destroy(int *, int *) { }
template<> inline void destroy(unsigned int *, unsigned int *) { }
template<> inline void destroy(long *, long *) { }
template<> inline void destroy(unsigned long *, unsigned long *) { }
template<> inline void destroy(float *, float *) { }
template<> inline void destroy(double *, double *) { }

template<> inline void destroy(char **, char **) { }
template<> inline void destroy(unsigned char **, unsigned char **) { }
template<> inline void destroy(short **, short **) { }
template<> inline void destroy(unsigned short **, unsigned short **) { }
template<> inline void destroy(int **, int **) { }
template<> inline void destroy(unsigned int **, unsigned int **) { }
template<> inline void destroy(long **, long **) { }
template<> inline void destroy(unsigned long **, unsigned long **) { }
template<> inline void destroy(float **, float **) { }
template<> inline void destroy(double **, double **) { }

///////////// COMMON [UN]INITIALIZATION METHODS ///////////

template<class InputIterator, class ForwardIterator>
ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last,
                   ForwardIterator result) {
//    return construct(&*(result + ()))
//    TODO:
    return result;
}

template<class InputIterator, class ForwardIterator>
ForwardIterator
copy(InputIterator first, InputIterator last,
                   ForwardIterator result) {
    while(first != last) {
        *result = *first;
        ++result;
        ++first;
    }
    return result;
}

template<class InputIterator, class BackwardIterator>
BackwardIterator
copy_backward(InputIterator first, InputIterator last,
                   BackwardIterator result) {
    while (first != last) {
        --result;
        --last;
        *result = *last;
    } 
    return result;
}

template<class InputIterator, class T>
void uninitialized_fill(InputIterator first, InputIterator last,
                   const T& val) {
    while(first != last) {
        *first = val;
        ++first;
    }
}


///////////// ALLOCATION OPERATOR IN MINISTL //////////////

typedef __default_alloc_template<true, 0> alloc;

template<class T, class Alloc = alloc>
class mini_alloc {
public:
    static void * allocate(size_t size) {
        return size == 0 ? 0 : Alloc::allocate(size * sizeof(T));
    }

    static  void * allocate(void) {
        return Alloc::allocate(sizeof(T));
    }

    static void deallocate(T * pointer, size_t size) {
        if (pointer) {
            Alloc::deallocate(pointer, size * sizeof(T));
        }
    }

    static void deallocate(T * pointer) {
        if (pointer) {
            Alloc::deallocate(pointer, sizeof(T));
        }
    }

};

END_MINISTL
