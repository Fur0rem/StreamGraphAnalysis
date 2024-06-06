"""
stream_graph_py - A Python module for stream graph analysis.
"""

FILE_NAME_1 = "stream_graph_py_module.so"
FILE_NAME_2 = "stream_graph_py.cpython-311-x86_64-linux-gnu.so"

def test_func_py(x):
    return x + 1

import os
import ctypes
import sys

# Load the shared library into ctypes
try:
    path = os.path.join(os.path.dirname(__file__))
    # go to ../src
    path = os.path.abspath(os.path.join(path, os.pardir, "src"))
    print(f"Loading shared library from: {path}")
    # load the shared library
    lib = ctypes.CDLL(os.path.join(path, FILE_NAME_2))
except Exception as e:
    print(f"Failed to load the shared library: {e}")
    sys.exit(1)

# Define test_func_c
def test_func_c(x):
    return lib.test_func_c(x)

class Test:
    
    obj : int
    def __init__(self, value):
        self.obj = lib.test_func_c(value)
    
    def increment(self):
        lib.Test_increment2(self.obj)
        
    def get_number(self):
        return lib.Test_get_number2(self.obj)