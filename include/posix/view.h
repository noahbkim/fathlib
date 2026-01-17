#ifndef POSIX_VIEW_H
#define POSIX_VIEW_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

Py_ssize_t _windows_unc_index(PyUnicodeObject *read);
PyUnicodeObject *_windows_drive(PyUnicodeObject *read);
PyObject *windows_drive(PyObject *module, PyObject *read);

#endif
