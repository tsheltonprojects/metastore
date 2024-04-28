#include <Python.h>

static PyObject *hello(PyObject *self, PyObject *args) {
    return PyUnicode_FromString("hello");
}

static PyMethodDef module_methods[] = {
    {"hello", hello, METH_NOARGS, "Return the string 'hello'"},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "example",
    NULL,
    -1,
    module_methods
};

PyMODINIT_FUNC PyInit_example(void) {
    return PyModule_Create(&module_def);
}
