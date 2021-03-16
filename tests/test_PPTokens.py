import unittest

from ctypes import *
from wrapper import *
from os import devnull


class TestIO(unittest.TestCase):
        
    def check_tokens(self, kind, tests):
        tokenkind = lib.ppTokenKindFromName(kind.encode("utf-8"))
        for input_, expected in tests.items():
            with self.subTest(f"{kind}, {input_}"):
                self.assertEqual(lib.checkPPToken(StrView(input_),tokenkind), expected)

    def test_checkPPToken(self):
        self.check_tokens("IDENTIFIER", {
            "test.abcd": 4,         # only "test" is found as identifier
            " test abcd": 0,        # space is not allowed in identifier
            "0ab3": 0,              # this is tokenized as number
            "_0ab3": 5,             # identifiers can contain digits
            "a\\u1234_bc": 10,      # universal character name (UCN) is part of identifier
            "a\\U12345678_bc": 14,  # same,
            "a\\u12xcv": 1,         # '\u12xc' is not a UCN, so it is not part of identifier
            "a\\U123456": -1,       # not enough bytes for the UCN, error
            "a\\bc": 1,             # other escape sequences are not allowed in identifiers
            "a\x83\x96z": 4,        # character >= 128 are part of identifiers (needed for UTF-8)
        })
        
        self.check_tokens("NUMBER", {
            "test.abcd": 0,   # no numbers here
            "567": 3,         # ok, integer
            "5.3e+6": 6,      # of, float
            "5.3+6": 3,       # '+' is not included if not preceded by 'e' (or 'p')
            ".3f": 3,         # starting with '.' is OK if followed by digit
            ".f": 0,          # '.' is only recognized if followed by digit
            "0xFF": 4,        # hexadecimal is OK
            "-53": 0,         # '-' represents the minus operator, so it is not part of the number
            "0c\\u12xcv": 2,  # '\u12xc' is not a UCN, so it is not part of the number
            ".5\\Ubc": -1,    # not enough bytes for the UCN
            "32'34": 2,       # apostrophe is not part of number by default
        })
        
        self.check_tokens("HEADER_NAME", {
            "<hello><test>": 7,    # '<hello>' found
            "<path/test.h>": 13,   # recognized as header name
            "\"test\"": 6,         # works with " too
            "<hello": -1,          # end not found
            "\"test": -1,          # end not found too
            "<hello\nworld>": -1,  # newline not allowed
        })
        
        self.check_tokens("CHAR_LITERAL", {
            "test": 0,          # no char-literal
            "'abc' 'def'": 5,   # 'abc' is found
            "''def": 2,         # empty char '' tokenized
            "test'a'": 0,       # test is not an encoding prefix
            "'a\\'b\\\nc'": 9,  # escaped ' and newline are OK
            "'a'_def": 3,       # by default, no C++ user-defined-suffix
            "'\\x32'": 6,       # valid
            "'abc": -1,         # no closing
            "'abc\ndef'": -1,   # unescaped newline not allowed
            "u'a'": 4,          # 'u' is OK as prefix
            "u8'a'": 0,         # 'u8' is not a valid prefix in C17
            "U8'a'": 0,         # not a valid prefix
        })
        
        self.check_tokens("STRING_LITERAL", {
            "test": 0,             # no char-literal
            "\"abc\" \"def\"": 5,  # 'abc' is found
            "\"\"def": 2,          # empty str "" tokenized
            "test\"a\"": 0,        # test is not an encoding prefix
            "\"a\\\"b\\\nc\"": 9,  # escaped "" and newline are OK
            "\"a\"_def": 3,        # stopping at closing "
            "\"\\x32\"": 6,        # valid
            "\"abc": -1,           # no closing
            "\"abc\ndef\"": -1,    # unescaped newline not allowed
            "u\"a\"": 4,           # 'u' is OK as prefix
            "u8\"a\"": 5,          # 'u8' too
            "U8\"a\"": 0,          # not a valid prefix
        })
        
        self.check_tokens("PUNCTUATOR", {
            "+= 3": 2,    # "+=" found
            "+ = 3": 1,   # "+" found
            ".....": 3,   # "..." found
            ".. hi": 1,   # "." found
            "/* 3 */": 1, # "/" found
            "<:hi:>": 2,  # "<:" found
        })
        
        self.check_tokens("WHITESPACE", {
            "abc": 0,             # no spaces
            "    def": 4,         # 4 spaces
            "\t \t def": 4,       # tabulations mixed with spaces ok
            "\t \r\n": 3,         # '\r' ok, '\n' not included
            "\v \r\\\n\v \n": 7,  # escaped newline included
        })
        
        self.check_tokens("COMMENT", {
            "// test": -1,                 # no new-line to end the inline comment
            "// test\n": 7,                # '\n' is not part of the comment
            "// hello \\\n world \n": 18,  # escaped newline is OK
            "/* test": -1,                # no closing */
            "/* test */": 10,             # "*/" is part of the comment
        })
    
    
    def check_logicalLine(self, line, expected):
        with self.subTest(line[:-1]):
            with lib.DynArray_make(400) as array:
                line = StrView(line) # MUST NOT be a temporary variable
                nb_tokens = lib.PPtokenizeLogicalLine(line, array) 
                self.assertEqual(nb_tokens * sizeof(PPToken), lib.DynArray_size(array))
                self.assertEqual(nb_tokens, len(expected))
                tokens = cast(array.begin, POINTER(PPToken))
                for token, expected in zip(tokens, expected):
                    with self.subTest(f"{token}, {expected}"):
                        self.assertEqual(token.repr, StrView(expected[0]))
                        self.assertEqual(lib.nameFromPPTokenKind(token.kind), expected[1].encode("utf-8"))
                        
            
        
    def test_PPtokenizeLogicalLine(self):
        
        # setup diagnostic handling
        
        with lib.IO_openOut(devnull.encode()) as f:
            lib.redirectDiagnostics(f)
            
            status = wrapFuncAndDiagnostic(
                    lambda: self.check_logicalLine("#include <stdio.h>\n", [
                ("#", "PUNCTUATOR"),
                ("include", "IDENTIFIER"),
                (" ", "WHITESPACE"),
                ("<stdio.h>", "HEADER_NAME"),
                ("\n", "NEWLINE")
            ]))
            self.assertEqual(status.value, DiagnosticStatus.OK)
            
            status = wrapFuncAndDiagnostic(
                    lambda: self.check_logicalLine("#define X(i) \\\n (++i)\n", [
                ("#", "PUNCTUATOR"),
                ("define", "IDENTIFIER"),
                (" ", "WHITESPACE"),
                ("X", "IDENTIFIER"),
                ("(", "PUNCTUATOR"),
                ("i", "IDENTIFIER"),
                (")", "PUNCTUATOR"),
                (" \\\n ", "WHITESPACE"),
                ("(", "PUNCTUATOR"),
                ("++", "PUNCTUATOR"),
                ("i", "IDENTIFIER"),
                (")", "PUNCTUATOR"),
                ("\n", "NEWLINE")
            ]))
            self.assertEqual(status.value, DiagnosticStatus.OK)
            
            status = wrapFuncAndDiagnostic(
                    lambda: self.check_logicalLine("a<b>c;\n", [
                ("a", "IDENTIFIER"),
                ("<", "PUNCTUATOR"),
                ("b", "IDENTIFIER"),
                (">", "PUNCTUATOR"),
                ("c", "IDENTIFIER"),
                (";", "PUNCTUATOR"),
                ("\n", "NEWLINE")
            ]))
            self.assertEqual(status.value, DiagnosticStatus.OK)
            
            # Warning on line not ending by '\n'
            status = wrapFuncAndDiagnostic(
                    lambda: self.check_logicalLine("#include <stdio.h>", [
                ("#", "PUNCTUATOR"),
                ("include", "IDENTIFIER"),
                (" ", "WHITESPACE"),
                ("<stdio.h>", "HEADER_NAME")
            ]))
            self.assertEqual(status.value, DiagnosticStatus.WARNING)
            
            # Error on incomplete token
            status = wrapFuncAndDiagnostic(
                    lambda: self.check_logicalLine("#include <stdio.h", []))
            self.assertEqual(status.value, DiagnosticStatus.ERROR)
            
            status = wrapFuncAndDiagnostic(
                    lambda: self.check_logicalLine("char const* a = 'b", []))
            self.assertEqual(status.value, DiagnosticStatus.ERROR)
            
            
            lib.redirectDiagnostics(None)
        
if __name__ == "__main__":
    unittest.main()