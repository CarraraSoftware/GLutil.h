#ifndef CARR_VEC_H_
#define CARR_VEC_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


// The user can define this macro to include only the macro functions 
// with the 'carr_' prefix, as to avoid name collisions.
// If this macro is not defined, all the versions without prefix
// will be included by default.
#ifndef CARR_VEC_FORCE_PREFIX

#define vec_new       carr_vec_new
#define vec_realloc   carr_vec_realloc
#define vec_init      carr_vec_init
#define vec_at        carr_vec_at
#define vec_append    carr_vec_append
#define vec_insert    carr_vec_insert
#define vec_delete    carr_vec_delete
#define vec_pop       carr_vec_pop
#define vec_swap      carr_vec_swap
#define vec_free      carr_vec_free
#define vec_clone     carr_vec_clone
#define vec_grow_cap  carr_vec_grow_cap
#define vec_grow      carr_vec_grow
#define vec_expandto  carr_vec_expandto

#define heapfy        carr_heapfy
#define heap_new      carr_heap_new
#define heap_increase carr_heap_increase
#define heap_pop      carr_heap_pop
#define heap_push     carr_heap_push
#define heap_balance  carr_heap_balance
#define heap_parent   carr_heap_parent
#define heap_left     carr_heap_left
#define heap_right    carr_heap_right

#define HeapCompareFunction CarrHeapCompareFunction

#endif // CARR_VEC_FORCE_PREFIX

/*-----------------------------------------------------------------------------+
 *                                                                             *
 *                           VEC STUFF                                         *
 *                                                                             *
 + ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------+
 *                                                                             *
 *  All the vec_* macros assume a struct in the form:                          *
 *  typedef struct {                                                           *
 *       void*  items                                                          *
 *       size_t len                                                            *
 *       size_t cap                                                            *
 *  } SomeUserStruct;                                                          *
 *  Where the items field can be of any type defined by the user.              *
 *                                                                             *
 +-----------------------------------------------------------------------------*/
                                                                   
#ifndef CARR_VEC_INITIAL_CAP
#define CARR_VEC_INITIAL_CAP 256
#endif  // CARR_VEC_INITIAL_CAP


#define carr_vec_realloc(vec, new_size) \
do {                                                                           \
    if ((vec)->cap < new_size) {                                               \
        (vec)->items = realloc((vec)->items, (sizeof(*(vec)->items) * (new_size))); \
    }                                                                          \
} while (0)

#define carr_vec_grow_cap(vec)                                                 \
    (vec)->cap > 0 ? (vec)->cap * 2 : CARR_VEC_INITIAL_CAP

#define carr_vec_grow(vec)                                                     \
do {                                                                           \
    size_t new_cap = carr_vec_grow_cap((vec));                                 \
    carr_vec_realloc((vec), new_cap);                                          \
    (vec)->cap = new_cap;                                                      \
} while (0)


#define carr_vec_expandto(vec, new_cap)                                        \
do {                                                                           \
    carr_vec_realloc((vec), new_cap);                                          \
    (vec)->cap = new_cap;                                                      \
} while (0)

#define carr_vec_new(vec_type)                                                 \
do {                                                                           \
    vec_type it = {                                                            \
        .items = NULL,                                                         \
        .len   = 0,                                                            \
        .cap   = 0,                                                            \
    };                                                                         \
    carr_vec_grow(&it);                                                        \
} while (0)

#define carr_vec_free(vec)                                                     \
do {                                                                           \
    free((vec)->items);                                                        \
    (vec)->items = NULL;                                                       \
} while (0)

#define carr_vec_clone(dst, src)                                               \
do {                                                                           \
    carr_vec_realloc((dst), (src)->cap);                                       \
    (dst)->cap = (src)->cap;                                                   \
    (dst)->len = (src)->len;                                                   \
    memcpy((dst)->items, (src)->items, (src)->len * sizeof((src)->items[0]));  \
} while (0)

#define carr_vec_init(vec)                                                     \
do {                                                                           \
    (vec)->items = NULL;                                                       \
    (vec)->len   = 0;                                                          \
    (vec)->cap   = 0;                                                          \
} while (0)

#define carr_vec_at(vec, idx)                                                  \
    (_carr_vec_assert((vec), (idx)), _carr_vec_at((vec), (idx)))

#define _carr_vec_assert(vec, idx)                                             \
    assert(idx < (vec)->len && idx >= 0)

#define _carr_vec_at(vec, idx)                                                 \
    (vec)->items[idx]

#define carr_vec_append(vec, item)                                             \
do {                                                                           \
    if ((vec)->len + 1 > (vec)->cap) {                                         \
        carr_vec_grow((vec));                                                  \
    }                                                                          \
    (vec)->items[(vec)->len++] = item;                                         \
} while (0)

#define carr_vec_insert(vec, item, idx)                                        \
do {                                                                           \
    if (idx > (vec)->len) {                                                    \
        printf(                                                                \
            "%s:%d:ERROR: Vec out of bound access. Len = %zu and index = %zu\n",\
            __FILE_NAME__, __LINE__, (vec)->len, idx                           \
        );                                                                     \
        exit(1);                                                               \
    }                                                                          \
                                                                               \
    if (idx == (vec)->len) {                                                   \
        carr_vec_append((vec), item);                                          \
        break;                                                                 \
    }                                                                          \
                                                                               \
    if ((vec)->len + 1 > (vec)->cap) {                                         \
        carr_vec_grow((vec));                                                  \
    }                                                                          \
                                                                               \
    size_t n   = ((vec)->len - idx) * sizeof( (vec)->items[0] );               \
    void* src  = (vec)->items + idx;                                           \
    void* dest = (vec)->items + (idx + 1);                                     \
    memmove(dest, src, n);                                                     \
    (vec)->items[idx] = item;                                                  \
    (vec)->len++;                                                              \
} while (0)


#define carr_vec_delete(vec, idx)                                              \
do {                                                                           \
    if (idx >= (vec)->len) {                                                   \
        break;                                                                 \
    }                                                                          \
    size_t n = ((vec)->len - (idx + 1)) * sizeof((vec)->items[0]);             \
    void* dest = (vec)->items + idx;                                           \
    void* src = (vec)->items + (idx + 1);                                      \
    memmove(dest, src, n);                                                     \
    (vec)->len--;                                                              \
} while (0)

#define carr_vec_pop(vec, res)                                                 \
do {                                                                           \
    if ((vec)->len == 0) {                                                     \
        printf(                                                                \
            "ERROR:%s:%d Cannot pop item out of an empty vector\n",            \
            __FILE_NAME__, __LINE__                                            \
        );                                                                     \
        exit(2);                                                               \
    }                                                                          \
    size_t idx = (vec)->len - 1;                                               \
    *(res) = carr_vec_at((vec), idx);                                          \
    carr_vec_delete((vec), idx);                                               \
} while (0)

#define carr_vec_swap(vec, i, j)                                               \
do {                                                                           \
    carr_vec_append((vec), carr_vec_at((vec), (i)));                           \
    (vec)->items[(i)] = carr_vec_at((vec), (j));                               \
    (vec)->items[(j)] = carr_vec_at((vec), (vec)->len-1);                      \
    (vec)->len--;                                                              \
} while (0)




/*-----------------------------------------------------------------------------+
 *                                                                             *
 *                           HEAP STUFF                                        *
 *                                                                             *
 +-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------+
 *                                                                             *
 *   All the heap_* macros assume a struct in the form:                        *
 *   typedef struct {                                                          *
 *        void*                   items                                        *
 *        size_t                  len                                          *
 *        size_t                  cap                                          *
 *        CarrHeapCompareFunction compare                                      *
 *   } SomeUserStruct;                                                         *
 *   Where the items field can be of any type defined by the user.             *
 *                                                                             *
 + ----------------------------------------------------------------------------*/


// This comparison function is the only addition on top of the regular 
// vec "interface" and it defines the comparison between items a and b, 
// such that if it returns true, a will be considered 'greater' than b. 
// 'Greater' here depends on the intended use of the heap by the user,
// it could be interpreted as 'has greater priority' as in a priority queue.
// In other words, this function should return true to indicate when 
// 'a' needs to be swapped with 'b' towards the top of the heap.
// In a MaxHeap, that would translate to a > b.
// In a MinHeap, a < b.
// The arguments are defined as void* to allow for arbitrary user structs.
// The file [examples/heap_queue.c] provides a complete example.
typedef bool(*CarrHeapCompareFunction)(void* a, void* b);

#define carr_heap_parent(i) (i - 1) >> 1
#define carr_heap_left(i)   ((i) << 1) | 1
#define carr_heap_right(i)  ((i + 1) << 1)


#define carr_heap_new(vec, func)                                               \
do {                                                                           \
    carr_vec_init((vec));                                                      \
    carr_vec_grow((vec));                                                      \
    (vec)->compare = func;                                                     \
} while (0) 

#define carr_heap_balance(h, idx)                                              \
do {                                                                           \
    struct { size_t* items; size_t  len; size_t  cap; } stack;                 \
    carr_vec_init(&stack);                                                     \
    carr_vec_grow(&stack);                                                     \
    carr_vec_append(&stack, idx);                                              \
                                                                               \
    while (stack.len > 0) {                                                    \
        size_t cur;                                                            \
        carr_vec_pop(&stack, &cur);                                            \
                                                                               \
        size_t l    = carr_heap_left(cur);                                     \
        size_t r    = carr_heap_right(cur);                                    \
        size_t best = cur;                                                     \
                                                                               \
        if (                                                                   \
            l < (h)->len &&                                                    \
            (h)->compare(                                                      \
                (void*)&_carr_vec_at((h), l),                                  \
                (void*)&_carr_vec_at((h), best)                                \
            )                                                                  \
        ) {                                                                    \
            best = l;                                                          \
        }                                                                      \
                                                                               \
        if (                                                                   \
            r < (h)->len &&                                                    \
            (h)->compare(                                                      \
                (void*)&_carr_vec_at((h), r),                                  \
                (void*)&_carr_vec_at((h), best)                                \
            )                                                                  \
        ) {                                                                    \
            best = r;                                                          \
        }                                                                      \
                                                                               \
        if (best != cur) {                                                     \
            carr_vec_swap((h), cur, best);                                     \
            carr_vec_append(&stack, best);                                     \
        }                                                                      \
    }                                                                          \
} while (0)

#define carr_heap_increase(h, idx, value)                                      \
do {                                                                           \
    h->items[(idx)] = value;                                                   \
    size_t par = carr_heap_parent((idx));                                      \
    while (                                                                    \
        par > 0 &&                                                             \
        (h)->compare(carr_vec_at((h), par), carr_vec_at((h), (idx)))           \
    ) {                                                                        \
        carr_vec_swap((h), (idx), par);                                        \
        par = carr_heap_parent(par);                                           \
    }                                                                          \
} while (0)


#define carr_heapfy(vec)                                                       \
do {                                                                           \
    int start = (vec)->len / 2 - 1;                                            \
    for (int i = start; i >= 0; i--) {                                         \
        carr_heap_balance((vec), i);                                           \
    }                                                                          \
} while (0)


#define carr_heap_push(h, value)                                               \
do {                                                                           \
    carr_vec_append((h), (value));                                             \
    carr_heap_increase((h), (h)->len-1, (value));                              \
} while (0)                                                                    \


#define carr_heap_pop(h, res)                                                  \
do {                                                                           \
    if ((h)->len == 1) {                                                       \
        carr_vec_pop((h), (res));                                              \
        break;                                                                 \
    }                                                                          \
    size_t last = (h)->len-1;                                                  \
    carr_vec_swap((h), (size_t)0, last);                                       \
    carr_vec_pop((h), (res));                                                  \
    carr_heap_balance((h), 0);                                                 \
} while (0)                                                                    \





#endif // CARR_VEC_H_
