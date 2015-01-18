#pragma once

#include <new>
#include <cstdlib>
#include "common.h"
#include "iterator.h"

void memcpy(void* src, void* dst, size_t copy_size);

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
        return ((size + __ALIGN - 1 ) & (__ALIGN - 1));
    }

private:
    static union obj * volatile free_list[__NFREELISTS] = { NULL };

    static inline size_t FREE_INDEX(size_t size) {
        return ((size + __ALIGN - 1)/(__ALIGN - 1));
    }

    // Fill the list when no free node in a list.
    // This will be invoked when alloc but free_list is empty.
    // Assume the size is already rounded up to *__ALIGN*
    static void * refill(size_t size) {
        int nobjs = __DEFAULT_NUM_CHUNK;

        char * chunk = chunk_alloc(size, nobjs); // Get chunk from the pool
        obj * volatile * cur_free_list;
        obj * result;
        obj * current_obj;

        // There's only one room, then take it and keep the free_list empty.
        if (nobjs == 1) return (chunk);

        cur_free_list = free_list + FREE_INDEX(size);
        result = (obj *)chunk;
        *cur_free_list = result + size; // Set 2nd as head, the 1st is taken.

        // Initialize all node in the list
        for (int i = 0; i < nobjs - 1; i++) {
            current_obj = result + i * size;
            current_obj->free_list_link = current_obj + size;
        }

        // Set the terminate
        current_obj = current_obj + size;
        current_obj->free_list_link = 0;

        return (result);
    }

    // Alloc a chunk memory with n*size. nobjs will be reduce when memory low
    static char * chunk_alloc(size_t size, int& nobjs) {
        char * result;
        size_t need_size = size * nobjs;
        size_t free_size = end_free - start_free;

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
                ((obj *)start_free)->free_list_link = cur_free_list;
                *cur_free_list = (obj *)start_free;
            }

            start_free = (char *)malloc(will_size);
            if (start_free == 0) {
                // There's nothing to be allocated, some crunks in the free_list
                // should be freed. Only consider size > current
                obj * volatile * cur_free_list;
                for (int i = size; i < __MAX_BYTES; i += __ALIGN) {
                    cur_free_list = free_list + FREE_INDEX(i);
                    if (*cur_free_list != 0) {
                        start_free = *cur_free_list;
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
char * __default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char * __default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

END_MINISTL
