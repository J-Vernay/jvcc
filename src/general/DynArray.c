#include "DynArray.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (buffer_size < DynArray_capacity(array)) {
        return;  // already enough space.
    }
    void* newptr = realloc(array->begin, buffer_size);
    if (!newptr) {
        free(array->begin);
        fputs("Error: no memory available for allocation.", stderr);
        exit(EXIT_FAILURE);
    }
    array->end = (char*)newptr + DynArray_size(array);
    array->begin = newptr;
    array->end_buffer = (char*)newptr + buffer_size;
}

void* DynArray_pushback(DynArray* array, void* data, size_t size) {
    assert(array != NULL);

    if (DynArray_size(array) + size > DynArray_capacity(array)) {
        // needs reallocation, reserving extra space for future appends
        size_t new_capacity = 2 * (size + DynArray_size(array));
        DynArray_reserve(array, new_capacity);
    }
    void* dest = array->end;
    if (data != NULL) {
        memcpy(dest, data, size);
    }
    array->end = dest + size;
    return dest;
}

void DynArray_popback(DynArray* array, size_t size) {
    assert(array != NULL);
    array->end = (char*)array->end - size;
}

// Inline functions already defined in DynArray.h, put here to generate external
// definitions.
extern size_t DynArray_size(DynArray const* array);
extern size_t DynArray_capacity(DynArray const* array);
