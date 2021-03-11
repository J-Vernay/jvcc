#ifndef JV_IO
#define JV_IO

#include <stdbool.h>
#include <stdio.h>

#include "DynArray.h"

// Opens a file. Alternatively, "stdin" and "stdout" are recognized.
// ("stderr" is not recognized because it is reserved for diagnostics.)
FILE* IO_openIn(char const* path);
FILE* IO_openOut(char const* path);

// Closes a file opened with openIO(). Correctly handles stdin/stdout/stderr.
void IO_close(FILE* file);

// Reads a line from 'file' and appends it to 'array'.
// Returns 'true' if a line could be read,
// 'false' if end-of-file was encountered.
bool IO_readLine(FILE* file, DynArray* array);

#endif