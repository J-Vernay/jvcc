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

// Reads a line from 'file' and appends it to 'array' as a null-terminated byte
// string. The resulting line will end with '\n', except if it is the last line
// of the file. Returns the number of bytes read from 'file'.
size_t IO_readLine(FILE* file, DynArray* array);

// Reads a logical line from 'file' and appends it to 'array' as a
// null-terminated byte string. A logical line is separated by a newline which
// is NOT preceded by a '\'. Returns the number of bytes read from 'file'.
size_t IO_readLogicalLine(FILE* file, DynArray* array);

#endif