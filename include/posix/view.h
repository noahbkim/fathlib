#ifndef POSIX_VIEW_H
#define POSIX_VIEW_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_posix_root(PyUnicodeObject *read);
PyObject *posix_root(PyObject *module, PyObject *read);

PyUnicodeObject *_posix_name(PyUnicodeObject *read);
PyObject *posix_name(PyObject *module, PyObject *read);

PyUnicodeObject *_posix_parent(PyUnicodeObject *read);
PyObject *posix_parent(PyObject *module, PyObject *read);

#endif
