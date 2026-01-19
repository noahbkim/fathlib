#ifndef WINDOWS_VIEW_H
#define WINDOWS_VIEW_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_windows_drive(PyUnicodeObject *arg);
PyObject *windows_drive(PyObject *module, PyObject *arg);

PyUnicodeObject *_windows_root(PyUnicodeObject *arg);
PyObject *windows_root(PyObject *module, PyObject *arg);

PyUnicodeObject *_windows_name(PyUnicodeObject *arg);
PyObject *windows_name(PyObject *module, PyObject *arg);

Py_ssize_t _windows_parent_index(PyUnicodeObject *arg);
PyObject *windows_parent(PyObject *module, PyObject *arg);

#endif
