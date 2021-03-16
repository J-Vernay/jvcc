#include "IO.h"

#include <string.h>

FILE* IO_openIn(char const* path) {
    if (strcmp(path, "stdin") == 0)
        return stdin;
    else
        return fopen(path, "rb");
}

FILE* IO_openOut(char const* path) {
    if (strcmp(path, "stdout") == 0)
        return stdout;
    else
        return fopen(path, "wb");
}

void IO_close(FILE* file) {
    if (file == stdin || file == stdout) return;
    fclose(file);
}

size_t IO_readLine(FILE* file, DynArray* array) {
    enum { BUFFER_SIZE = 500 };
    char buffer[BUFFER_SIZE];

    size_t nb_read = 0;
    for (;;) {
        if (fgets(buffer, BUFFER_SIZE, file) == NULL) break;

        size_t size = strlen(buffer);
        nb_read += size;
        // size >= 1, because fgets() returns NULL if no bytes read.
        if (size < BUFFER_SIZE || buffer[size - 1] == '\n') {
            // Found either end-of-line or end-of-file.
            // Copying 'size + 1' bytes, which include '\0'.
            DynArray_pushback(array, buffer, size + 1);
            break;
        } else {
            // Buffer full before end of line.
            // Copying 'size' bytes, not including '\0'.
            DynArray_pushback(array, buffer, size);
            continue;
        }
    }
    return nb_read;
}

size_t IO_readLogicalLine(FILE* file, DynArray* array) {
    size_t total_read = 0;

    while (true) {
        size_t nb_read = IO_readLine(file, array);
        total_read += nb_read;

        // Check if ends with \ + \n
        if (nb_read >= 2)
            if ('\n' == ((char*)array->end)[-2] &&
                '\\' == ((char*)array->end)[-3]) {
                // escaped newline
                --array->end;  // removing '\0'
                continue;
            }
        // not escaped newline, returns
        return total_read;
    }
}