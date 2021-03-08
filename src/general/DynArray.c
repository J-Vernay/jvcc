#include "DynArray.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE_SIZE(a, b) ((char const*)(b) - (char const*)(a))
#define ARRAY_SIZE(a) (size_t) BYTE_SIZE(a->begin, a->end)
#define ARRAY_CAPACITY(a) (size_t) BYTE_SIZE(a->begin, a->end_buffer)

DynArray DynArray_make(size_t buffer_size) {
    DynArray array = {0};
    DynArray_reserve(&array, buffer_size);
    return array;
}

void DynArray_destroy(DynArray* array) {
    assert(array != NULL);
    free(array->begin);
    memset(array, 0, sizeof(DynArray));
}

void DynArray_reserve(DynArray* array, size_t buffer_size) {
    assert(array != NULL);
    if (buffer_size < ARRAY_CAPACITY(array)) {
        return;  // already enough space.
    }
    void* newptr = realloc(array->begin, buffer_size);
    if (!newptr) {
        free(array->begin);
        fputs("Error: no memory available for allocation.", stderr);
        exit(EXIT_FAILURE);
    }
    array->end = (char*)newptr + ARRAY_SIZE(array);
    array->begin = newptr;
    array->end_buffer = (char*)newptr + buffer_size;
}

void* DynArray_pushback(DynArray* array, void* data, size_t size) {
    assert(array != NULL);

    if (array->end + size > array->end_buffer) {
        // needs reallocation, reserving extra space for future appends
        size_t new_capacity = 2 * (size + ARRAY_CAPACITY(array));
        DynArray_reserve(array, new_capacity);
    }
    void* dest = array->end;
    if (data != NULL) {
        memcpy(dest, data, size);
    }
    array->end = dest;
    return dest;
}

void DynArray_popback(DynArray* array, size_t size) {
    assert(array != NULL);
    array->end = (char*)array->end - size;
}