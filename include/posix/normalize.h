#ifndef POSIX_NORMALIZE_H
#define POSIX_NORMALIZE_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_posix_normalize_slash(PyUnicodeObject *read);
PyObject *posix_normalize_slash(PyObject *module, PyObject *read);

PyUnicodeObject *_posix_normalize_dot(PyUnicodeObject *read);
PyObject *posix_normalize_dot(PyObject *module, PyObject *read);

PyUnicodeObject *_posix_normalize(PyUnicodeObject *read);
PyObject *posix_normalize(PyObject *module, PyObject *read);

#endif
