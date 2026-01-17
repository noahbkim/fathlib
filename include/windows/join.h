#ifndef WINDOWS_JOIN_H
#define WINDOWS_JOIN_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_windows_join(PyObject *items, int count);
PyObject *windows_join(PyObject *module, PyObject *items);

#endif
