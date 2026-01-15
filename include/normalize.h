#ifndef NORMALIZE_H
#define NORMALIZE_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_normalize_slash(PyUnicodeObject *read);
PyObject *normalize_slash(PyObject *module, PyObject *read);

PyUnicodeObject *_normalize_dot(PyUnicodeObject *read);
PyObject *normalize_dot(PyObject *module, PyObject *read);

#endif
