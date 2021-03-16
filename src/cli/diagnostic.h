#ifndef JV_DIAGNOSTIC
#define JV_DIAGNOSTIC

#include <setjmp.h>
#include <stdio.h>

// This is the API to emit warnings and errors, and add context.
// Every thing is thread-local for now.

// Set where the Diagnostics will be outputted.
// By default, it is in stderr.
// Passing nullptr will redirect to stderr.
void redirectDiagnostics(FILE* f);

// Pushes a message that will be displayed on warning/error.
// The returned value must be passed to 'jvcc_popCtx' when the message is not
// relevant anymore.
void* pushContext(char const* format, ...);

// Cancels a message pushed by jvcc_pushCtx.
void popContext(void*);

// This will display the warning with its context.
// After the warning is displayed, the control returns to the caller.
void emitWarning(char const* format, ...);

// This will display the error with its context.
// After the error is displayed, the handler setup by setErrorHandler() is
// called. If the handler returns, exit(EXIT_FAILURE) will be called next.
_Noreturn void emitError(char const* format, ...);

typedef void (*DiagnosticHandler)(void* userdata);

// Setup an handler to be called by emitWarning() after printing warning.
// 'userdata' will be passed to 'handler' when called.
// 'handler' can be NULL, in which case emitWarning will only display text.
void setWarningHandler(DiagnosticHandler handler, void* userdata);

// Setup an handler to be called by emitError() after printing error.
// 'userdata' will be passed to 'handler' when called.
// 'handler' can be NULL, in which case emitError will call exit(EXIT_FAILURE).
void setErrorHandler(DiagnosticHandler handler, void* userdata);

// Will call longjmp(jump_buffer, 1), considering jump_buffer as a jmp_buf.
// Can be used for setErrorHandler. Example:
//     jmp_buf jump_buffer;
//     setErrorHandler(&callLongjmp, &jump_buffer);
//     if (setjmp(jump_buffer) == 0) {
//         ... actual logic ...
//     } else {
//         ... on error ...
//     }
//     setErrorHandler(NULL, NULL);
//
_Noreturn void callLongjmp(void* jump_buffer);

// Wraps a function call so Warning and Error are provided with an error code
// instead of callbacks.
typedef enum DiagnosticStatus {
    DiagnosticStatus_OK = 0,
    DiagnosticStatus_WARNING = 1,
    DiagnosticStatus_ERROR = 2
} DiagnosticStatus;
DiagnosticStatus wrapFuncAndDiagnostic(void (*func)(void* userdata),
                                       void* userdata);

#endif