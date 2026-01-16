#ifndef WINDOWS_NORMALIZE_H
#define WINDOWS_NORMALIZE_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_windows_normalize_slash(PyUnicodeObject *read);
PyObject *windows_normalize_slash(PyObject *module, PyObject *read);

PyUnicodeObject *_windows_normalize_dot(PyUnicodeObject *read);
PyObject *windows_normalize_dot(PyObject *module, PyObject *read);

PyUnicodeObject *_windows_normalize(PyUnicodeObject *read);
PyObject *windows_normalize(PyObject *module, PyObject *read);

#endif
