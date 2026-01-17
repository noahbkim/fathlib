#ifndef WINDOWS_VIEW_H
#define WINDOWS_VIEW_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_windows_drive(PyUnicodeObject *read);
PyObject *windows_drive(PyObject *module, PyObject *read);

#endif
