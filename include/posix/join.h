#ifndef POSIX_JOIN_H
#define POSIX_JOIN_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_posix_join(PyObject **args, Py_ssize_t nargs);
PyObject *posix_join(PyObject *module, PyObject **args, Py_ssize_t nargs);

PyUnicodeObject *_posix_concat(PyObject **args, Py_ssize_t nargs);
PyObject *posix_concat(PyObject *module, PyObject **args, Py_ssize_t nargs);

#endif
