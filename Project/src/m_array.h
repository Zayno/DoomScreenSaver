

#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "i_system.h"

#define M_ARRAY_INIT_CAPACITY   128

typedef struct
{
    int     capacity;
    int     size;
    char    buffer[];
} m_array_buffer_t;

inline static m_array_buffer_t *array_ptr(const void *v)
{
    return (m_array_buffer_t *)((char *)v - offsetof(m_array_buffer_t, buffer));
}

inline static int array_size(const void *v)
{
    return (v ? array_ptr(v)->size : 0);
}

inline static int array_capacity(const void *v)
{
    return (v ? array_ptr(v)->capacity : 0);
}

inline static void array_clear(const void *v)
{
    if (v)
        array_ptr(v)->size = 0;
}

#define array_grow(v, n) \
    ((v) = M_ArrayGrow((v), sizeof(*(v)), n))

#define array_push(v, e)                                                    \
    {                                                                       \
        if (!(v))                                                           \
            (v) = M_ArrayGrow((v), sizeof(*(v)), M_ARRAY_INIT_CAPACITY);    \
        else if (array_ptr((v))->size == array_ptr((v))->capacity)          \
            (v) = M_ArrayGrow((v), sizeof(*(v)), array_ptr((v))->capacity); \
                                                                            \
        (v)[array_ptr((v))->size++] = (e);                                  \
    }

#define array_free(v)         \
    if (v)                    \
    {                         \
        free(array_ptr((v))); \
        (v) = NULL;           \
    }

#define array_foreach(ptr, v) \
    for (ptr = (v); ptr < (v) + array_size(v); ptr++)

inline static void *M_ArrayGrow(void *v, size_t esize, int n)
{
    m_array_buffer_t    *p;

    if (v)
    {
        p = array_ptr(v);
        p = I_Realloc(p, sizeof(m_array_buffer_t) + ((size_t)p->capacity + n) * esize);
        p->capacity += n;
    }
    else
    {
        p = I_Malloc(sizeof(m_array_buffer_t) + n * esize);
        p->capacity = n;
        p->size = 0;
    }

    return p->buffer;
}
