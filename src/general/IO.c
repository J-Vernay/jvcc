#include "IO.h"

#include <string.h>

FILE* openInput(char const* path) {
    if (strcmp(path, "stdin") == 0)
        return stdin;
    else
        return fopen(path, "rb");
}

FILE* openOutput(char const* path) {
    if (strcmp(path, "stdout") == 0)
        return stdout;
    else
        return fopen(path, "wb");
}

void closeIO(FILE* file) {
    if (file == stdin || file == stdout) return;
    fclose(file);
}

bool readLine(FILE* file, DynArray* array) {
    enum { BUFFER_SIZE = 500 };
    char buffer[BUFFER_SIZE];
    for (;;) {
        if (fgets(buffer, BUFFER_SIZE, file) == NULL) return false;

        size_t size = strlen(buffer);
        // size >= 1, because fgets() returns NULL if no bytes read.
        if (buffer[size - 1] == '\n') {
            // Found the end of line.
            // Copying 'size + 1' bytes, which include '\0'.
            DynArray_pushback(array, buffer, size + 1);
            return true;
        } else if (size == BUFFER_SIZE - 1) {
            // Buffer full before end of line.
            // Copying 'size' bytes, not including '\0'.
            DynArray_pushback(array, buffer, size);
            continue;
        } else {
            // Found end-of-file.
            // TODO: Emit warning because last line did not have '\n'.
            char* dest = DynArray_pushback(array, NULL, size + 2);
            memcpy(dest, buffer, size);
            memcpy(dest + size, "\n", 2);
            return true;
        }
    }
}