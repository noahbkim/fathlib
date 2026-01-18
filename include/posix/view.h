#ifndef POSIX_VIEW_H
#define POSIX_VIEW_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_posix_root(PyUnicodeObject *arg);
PyObject *posix_root(PyObject *module, PyObject *arg);

PyUnicodeObject *_posix_name(PyUnicodeObject *arg);
PyObject *posix_name(PyObject *module, PyObject *arg);

#endif
