#ifndef JOIN_H
#define JOIN_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_fspath(PyObject *item);

PyUnicodeObject *_join_posix(PyObject *items, int count);
PyObject *join_posix(PyObject *module, PyObject *items);

#endif
