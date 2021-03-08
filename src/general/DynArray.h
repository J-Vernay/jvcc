#ifndef JV_DYNARRAY_H
#define JV_DYNARRAY_H

#include <stddef.h>

// This header defines the struct DynArray, which represents a general-purpose
// growable heap-allocated array.

// DynArray can be zero-initialized.
typedef struct DynArray {
    void* begin;       // The actual array.
    void* end;         // [begin;end[ is the logical range of the array.
    void* end_buffer;  // [begin;end_buffer[ is the current buffer.
} DynArray;

// Creates an empty DynArray with a buffer of 'buffer_size' bytes.
DynArray DynArray_make(size_t buffer_size);

// Frees the memory and reset '*array' to {0}.
void DynArray_destroy(DynArray* array);

// Reserves at least 'buffer_size' bytes for the buffer, which means no extra
// allocations will be performed until the array size exceeds 'buffer_size'.
void DynArray_reserve(DynArray* array, size_t buffer_size);

// Appends [data;data+size[ at the end of the array and returns a pointer to the
// appended data. If 'data == NULL', the array is correctly resized but no data
// is copied into the array.
// EXAMPLES:
//   DynArray_pushback(&array, &my_int, sizeof(int));    // Add my_int.
//   memset(DynArray_pushback(&array, NULL, 10), 0, 10); // Add 10 null bytes.
void* DynArray_pushback(DynArray* array, void* data, size_t size);

// Removes the last 'size' bytes of the array.
void DynArray_popback(DynArray* array, size_t size);

#endif