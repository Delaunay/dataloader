 
from ctypes import cdll
import pathlib


def wrap_bar():
    foo = cdll.LoadLibrary(str(pathlib.Path(__file__).with_name('cpploader.so')))
    return foo.module()
