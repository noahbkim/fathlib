#ifndef WINDOWS_JOIN_H
#define WINDOWS_JOIN_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_windows_join(PyObject **args, Py_ssize_t nargs);
PyObject *windows_join(PyObject *module, PyObject **args, Py_ssize_t nargs);

PyUnicodeObject *_windows_concat(PyObject **args, Py_ssize_t nargs);
PyObject *windows_concat(PyObject *module, PyObject **args, Py_ssize_t nargs);

#endif
