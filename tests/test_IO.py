import unittest

from ctypes import *
from wrapper import lib
from tempfile import gettempdir

class TestIO(unittest.TestCase):
        
    def test_readLine(self):
        # This test compares the output of IO_readLine with the output of Python's .readlines
        
        # first let's read lines with Python
        with open("wrapper.py", "rb") as f:
            expected_lines = f.readlines()
        expected_lines[-1] += b"\n" # feature of IO_readLine: last line is '\n' terminated
        
        # then with IO_readLine
        with lib.DynArray_make(10) as array, lib.IO_openIn(b"wrapper.py") as f:  
            for expected_line in expected_lines:
                # returns true if a line was read
                self.assertTrue(lib.IO_readLine(f, array))
                
                line = cast(array.begin, c_char_p).value # converting void* to char*
                self.assertEqual(line, expected_line)
                
                array.end = array.begin # reset array
            # no more lines shall be available
            self.assertFalse(lib.IO_readLine(f, array))
     
        
        
if __name__ == "__main__":
    unittest.main()