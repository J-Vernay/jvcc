#include "diagnostic.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "general/DynArray.h"

static _Thread_local DynArray contexts = {0};

static _Thread_local FILE* diagnosticsOutput = NULL;

void redirectDiagnostics(FILE* f) { diagnosticsOutput = f; }

static inline size_t nextAligned(size_t size, size_t align) {
    // because align is a power of two, align_mask will be a correct mask.
    size_t align_mask = align - 1;
    // '& align_mask' is the same as '% align'.
    // The last '(...) & align_mask' means '(...) == align ? 0 : (...)'.
    size_t padding_needed = (align - (size & align_mask)) & align_mask;
    return size + padding_needed;
}

void* pushContext(char const* format, ...) {
    va_list args1, args2;
    va_start(args1, format);
    va_copy(args2, args1);

    int msg_size = vsnprintf(NULL, 0, format, args1);
    if (msg_size < 0) return NULL;

    // getting space for int + msg, aligned on int
    size_t buffer_size = nextAligned(sizeof(int) + msg_size + 1, _Alignof(int));
    void* buffer = DynArray_pushback(&contexts, NULL, buffer_size);

    *(int*)buffer = buffer_size;
    vsprintf((char*)buffer + sizeof(int), format, args2);

    va_end(args1);
    va_end(args2);

    return buffer;
}

void popContext(void* end_before) { contexts.end = end_before; }

// Returns the number of indentation needed for actual message.
static void printContext() {
    int indentation = 2;
    void *it = contexts.begin, *end = contexts.end;
    while (it < end) {
        int buffer_size = *(int*)it;
        fprintf(diagnosticsOutput, "%*s %s\n", indentation, "",
                (char*)it + sizeof(int));
        it = (char*)it + buffer_size;
    }
}

static _Thread_local DiagnosticHandler warningHandler = NULL;
static _Thread_local void* warningHandlerData = NULL;
static _Thread_local DiagnosticHandler errorHandler = NULL;
static _Thread_local void* errorHandlerData = NULL;

void setWarningHandler(DiagnosticHandler handler, void* userdata) {
    warningHandler = handler;
    warningHandlerData = userdata;
}

void setErrorHandler(DiagnosticHandler handler, void* userdata) {
    errorHandler = handler;
    errorHandlerData = userdata;
}

void emitWarning(char const* format, ...) {
    if (diagnosticsOutput == NULL) diagnosticsOutput = stderr;
    fputs("WARNING:\n", diagnosticsOutput);
    printContext();
    va_list args1;
    va_start(args1, format);
    vfprintf(diagnosticsOutput, format, args1);
    va_end(args1);
    fputs("\n", diagnosticsOutput);
    if (warningHandler != NULL) (*warningHandler)(warningHandlerData);
}

_Noreturn void emitError(char const* format, ...) {
    if (diagnosticsOutput == NULL) diagnosticsOutput = stderr;
    fputs("ERROR:\n", diagnosticsOutput);
    printContext();
    va_list args1;
    va_start(args1, format);
    vfprintf(diagnosticsOutput, format, args1);
    va_end(args1);
    fputs("\n", diagnosticsOutput);
    if (errorHandler != NULL) (*errorHandler)(errorHandlerData);
    exit(EXIT_FAILURE);
}

_Noreturn void callLongjmp(void* jump_buffer) {
    longjmp(*(jmp_buf*)jump_buffer, 1);
}

static void wrappedWarningHandler(void* flag) { *(bool*)flag = true; }

DiagnosticStatus wrapFuncAndDiagnostic(void (*func)(void* userdata),
                                       void* userdata) {
    jmp_buf jump_buffer;
    bool warning_flag = false, error_flag = false;
    if (setjmp(jump_buffer) == 0) {
        setWarningHandler(&wrappedWarningHandler, &warning_flag);
        setErrorHandler(&callLongjmp, &jump_buffer);
        (*func)(userdata);
    } else {
        error_flag = true;
    }
    if (error_flag)
        return DiagnosticStatus_ERROR;
    else if (warning_flag)
        return DiagnosticStatus_WARNING;
    else
        return DiagnosticStatus_OK;
}