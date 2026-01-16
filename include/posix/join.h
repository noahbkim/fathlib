#ifndef POSIX_JOIN_H
#define POSIX_JOIN_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_posix_join(PyObject *items, int count);
PyObject *posix_join(PyObject *module, PyObject *items);

#endif
