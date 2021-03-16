/*
This file contains the extraction of tokens from the source file (lexing).
It corresponds to both phase 2 and phase 3 of translation in both C and C++.
See also: https://en.cppreference.com/w/c/language/translation_phases .
The expected input is UTF-8 or other ASCII compatible encodings.

CAUTION:
Escaped newlines are considered as whitespaces, that is,
'+' '\\' '\n' '=' will be considered as "+ =" (and not "+=" as specified in the
standard). This applies inside identifiers, numbers and punctuators. Escaped
newlines inside char or string literal are OK. This is done to keep the logic
simple.
*/

#ifndef JV_PPTOKENS
#define JV_PPTOKENS

#ifdef __cplusplus
extern "C" {
#endif

#include "StrView.h"
#include "general/DynArray.h"

// Each extracted token has a kind.
// At this point, the tokens are not semantically analyzed.
// Keywords are left as identifiers.
// Numbers include integers, floats, and also invalid constants such as
// "1..E+3.foo". No validation is done at this stage.
typedef enum PPTokenKind {
    PPToken_HEADER_NAME,
    PPToken_IDENTIFIER,
    PPToken_NUMBER,
    PPToken_CHAR_LITERAL,
    PPToken_STRING_LITERAL,
    PPToken_PUNCTUATOR,
    PPToken_WHITESPACE,
    PPToken_NEWLINE,
    PPToken_COMMENT,
} PPTokenKind;

typedef struct PPToken {
    StrView repr;
    PPTokenKind kind;
} PPToken;

// High-level function to transform a logical line of source file to a list of
// PPToken. The sequence of PPToken is appended to 'out'.
// Returns the number of tokens appended.
// EMIT AN ERROR if a token is incomplete (header-name, string constant, etc).
// EMIT A WARNING if a line is incomplete (not ending by '\n').
size_t PPtokenizeLogicalLine(StrView line, DynArray* out);

// Low-level function to check if the source starts with a token of specified
// 'kind'. 'src' must have at least one character.
// Returns the number of bytes of the token, or 0 if the source does not
// start with the specified token kind.
// If the token is incomplete (i.e. string literal not closed), returns -1.
int checkPPToken(StrView src, PPTokenKind kind);

// Utility functions to convert from PPTokenKind to its name, and vice-versa.
// In ppTokenKindFromName(), 'name' is expected to be one of HEADER_NAME,
// IDENTIFIER, etc. No handling is done for invalid names.
PPTokenKind ppTokenKindFromName(char const* name);
char const* nameFromPPTokenKind(PPTokenKind kind);

#ifdef __cplusplus
}
#endif

#endif