
from ctypes import * #hi
import subprocess #hello
from glob import glob
from pathlib import Path


lib = None
exe = None

def _init():
    libpath = None
    exepath = None
    
    for candidate in Path("../build").iterdir():
        name = candidate.name.split(".")[0]
        if name == "jvcc-shared" or name == "libjvcc-shared":
            if libpath != None:
                raise Exception(f"Two candidates for jvcc-shared: {libpath} and {candidate}")
            libpath = candidate
        elif name == "jvcc":
            if exepath != None:
                raise Exception(f"Two candidates for jvcc: {exepath} and {candidate}")
            exepath = candidate
    
    if libpath == None:
        raise Exception("No candidates found for jvcc-shared in ../build")
    if exepath == None:
        raise Exception("No candidates found for jvcc in ../build")
    
    global exe, lib
    exe = exepath
    lib = cdll.LoadLibrary(libpath)
    
_init()

def _wrap(restype, name, *argtypes):
    func = lib.__getattr__(name)
    func.restype = restype
    func.argtypes = argtypes
    lib.__setattr__(name, func)
    return func


def _destructor(func): # for types deriving from Structure
    def _exit(self,*_):
        func(pointer(self))
    def f(c):
        c.__enter__ = lambda self: self
        c.__exit__ = _exit
        return c
    return f
    
def _ptr_destructor(func): # for types deriving from c_void_p
    def _exit(self,*_):
        func(self)
    def f(c):
        c.__enter__ = lambda self: self
        c.__exit__ = _exit
        return c
    return f 

###### StrView.h ######

class StrView(Structure):
    _fields_ = [("begin",c_void_p), ("size",c_ulonglong)]
    
    def __init__(self, string):
        self.begin = cast(c_char_p(string.encode("utf-8")), c_void_p)
        self.size = len(string)
    
    def __eq__(self, other):
        if isinstance(other, StrView):
            return lib.jvstr_equal(self, other)
        return NotImplemented
        
    def __str__(self):
        return string_at(self.begin, self.size).decode("utf-8")
        
def StrView_make(string):
    return StrView(cast(c_char_p(string), c_void_p), len(string))


###### general/DynArray.h ######

@_destructor(lib.DynArray_destroy)
class DynArray(Structure):
    _fields_ = [("begin",c_void_p), ("end",c_void_p), ("end_buffer",c_void_p)]


_wrap(DynArray, "DynArray_make", c_ulonglong)
_wrap(None, "DynArray_destroy", POINTER(DynArray))
_wrap(c_ulonglong, "DynArray_size", POINTER(DynArray))


###### general/IO.h ######

@_ptr_destructor(lib.IO_close)
class CFilePtr(c_void_p):
    pass # opaque structure
    
_wrap(CFilePtr, "IO_openIn", c_char_p)
_wrap(CFilePtr, "IO_openOut", c_char_p)
_wrap(None, "IO_close", CFilePtr)
_wrap(c_ulonglong, "IO_readLine", CFilePtr, POINTER(DynArray))

###### cli/diagnostic.h ######

DiagnosticHandler = CFUNCTYPE(None, c_void_p)
_wrap(None, "setWarningHandler", DiagnosticHandler, c_void_p)
_wrap(None, "setErrorHandler", DiagnosticHandler, c_void_p)
_wrap(None, "redirectDiagnostics", CFilePtr)

class DiagnosticStatus(c_int):
    OK = 0
    WARNING = 1
    ERROR = 2
_WrappedFunction = CFUNCTYPE(None, c_void_p)
_wrap(DiagnosticStatus, "wrapFuncAndDiagnostic", _WrappedFunction, c_void_p)

def wrapFuncAndDiagnostic(f):
    f2 = lambda _: f()
    func = _WrappedFunction(f2)
    return lib.wrapFuncAndDiagnostic(func, None)
    

###### c17/PPTokens.h ######

class PPTokenKind(c_int):
    pass

class PPToken(Structure):
    _fields_ = [("repr",StrView), ("kind", PPTokenKind)]

    def __str__(self):
        return f"(\"{self.repr}\", {lib.nameFromPPTokenKind(self.kind).decode('utf-8')})"
        

_wrap(c_ulonglong, "PPtokenizeLogicalLine", StrView, POINTER(DynArray))
_wrap(c_int, "checkPPToken", StrView, PPTokenKind)
_wrap(PPTokenKind, "ppTokenKindFromName", c_char_p)
_wrap(c_char_p, "nameFromPPTokenKind", PPTokenKind)
