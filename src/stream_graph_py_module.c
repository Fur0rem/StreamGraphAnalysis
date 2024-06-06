#include <Python.h>

// Function to add 2 to an integer

int test_func_c(int x) {
	return x + 9;
}

static PyObject* test_func_c_wrapper(PyObject* self, PyObject* args) {
	int num;

	// Parse the input argument
	if (!PyArg_ParseTuple(args, "i", &num)) {
		return NULL;
	}

	// Call the C function
	num = test_func_c(num);

	// Return the result as a Python object
	return Py_BuildValue("i", num);
}

// Structure for the Test class
typedef struct {
	PyObject HEAD;
	int number;
} TestObject;

// Method to create a new Test object
static PyObject* Test_new2(PyTypeObject* type, PyObject* args, PyObject* kwds) {
	TestObject* self;

	// Create the object
	self = (TestObject*)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->number = 0;
	}

	// Return the object
	return (PyObject*)self;
}

// Method to increment the number
static PyObject* Test_increment2(TestObject* self, PyObject* args) {
	// Increment the number
	self->number += 1;

	// Return None
	Py_RETURN_NONE;
}

// Method to get the number
static PyObject* Test_get_number2(TestObject* self, PyObject* args) {
	// Return the number as a Python object
	return Py_BuildValue("i", self->number);
}

// Method definitions for the Test class
static PyMethodDef Test_methods2[] = {
	{"increment", (PyCFunction)Test_increment2, METH_NOARGS, "Increment the number"},
	{"get_number", (PyCFunction)Test_get_number2, METH_NOARGS, "Get the number"},
	{NULL, NULL, 0, NULL} // Sentinel
};

// Method to deallocate a Test object
static void Test_dealloc2(TestObject* self) {
	Py_TYPE(self)->tp_free((PyObject*)self);
}

// Type definition for the Test class
static PyTypeObject TestType = {
	PyVarObject_HEAD_INIT(NULL, 0).tp_name = "stream_graph_py.Test",
	.tp_doc = "Test objects",
	.tp_basicsize = sizeof(TestObject),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = Test_new2,
	.tp_dealloc = Test_dealloc2,
};

// Method to create a new Test object
static PyObject* Test_new(PyObject* self, PyObject* args) {
	TestObject* obj = PyObject_New(TestObject, &TestType);
	obj->number = 0;
	return (PyObject*)obj;
}

// Method to increment the number
static PyObject* Test_increment(PyObject* self, PyObject* args) {
	// Increment the number
	TestObject* obj = (TestObject*)self;
	obj->number += 1;

	// Return None
	Py_RETURN_NONE;
}

// Method to get the number
static PyObject* Test_get_number(PyObject* self, PyObject* args) {
	// Return the number as a Python object
	TestObject* obj = (TestObject*)self;
	return Py_BuildValue("i", obj->number);
}

// Method to deallocate a Test object
static void Test_dealloc(PyObject* self, PyObject* args) {
	PyObject_Del(self);
}

// Functions defined in the module
static PyMethodDef stream_graph_py_methods[] = {
	{"test_func_c", test_func_c_wrapper, METH_VARARGS, "Add 2 to an integer"},
	{"Test_new2", Test_new2, METH_NOARGS, "Create a new Test object"},
	{"Test_increment2", Test_increment2, METH_VARARGS, "Increment the number"},
	{"Test_get_number2", Test_get_number2, METH_VARARGS, "Get the number"},
	{NULL, NULL, 0, NULL} // Sentinel
};

// Module definition
static struct PyModuleDef stream_graph_py = {
	PyModuleDef_HEAD_INIT, .m_name = "stream_graph_py",			 .m_doc = "Example module that adds 1 to an integer",
	.m_size = -1,		   .m_methods = stream_graph_py_methods,
};

// Module initialization function
PyMODINIT_FUNC PyInit_stream_graph_py(void) {

	printf("Hello from PyInit_stream_graph_py\n");
	PyObject* module;

	// Create the module
	module = PyModule_Create(&stream_graph_py);
	if (module == NULL) {
		return NULL;
	}

	printf("Returning from PyInit_stream_graph_py\n");

	return module;
}