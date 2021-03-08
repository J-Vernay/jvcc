#ifndef JV_IO
#define JV_IO

#include <stdbool.h>
#include <stdio.h>

#include "DynArray.h"

// Opens a file. Alternatively, "stdin" and "stdout" are recognized.
// ("stderr" is not recognized because it is reserved for diagnostics.)
FILE* openInput(char const* path);
FILE* openOutput(char const* path);

// Closes a file opened with openIO(). Correctly handles stdin/stdout/stderr.
void closeIO(FILE* file);

// Reads a line from 'file' and appends it to 'array'.
// Returns 'true' if a line could be read,
// 'false' if end-of-file was encountered.
bool readLine(FILE* file, DynArray* array);

#endif