
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


@_destructor(lib.DynArray_destroy)
class DynArray(Structure):
    _fields_ = [("begin",c_void_p), ("end",c_void_p), ("end_buffer",c_void_p)]

@_ptr_destructor(lib.IO_close)
class CFilePtr(c_void_p):
    pass # opaque structure

_wrap(DynArray, "DynArray_make", c_ulonglong)
_wrap(None, "DynArray_destroy", POINTER(DynArray))

_wrap(CFilePtr, "IO_openIn", c_char_p)
_wrap(CFilePtr, "IO_openOut", c_char_p)
_wrap(None, "IO_close", CFilePtr)
_wrap(c_bool, "IO_readLine", CFilePtr, POINTER(DynArray))