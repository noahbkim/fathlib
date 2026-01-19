#ifndef COMMON_H
#define COMMON_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

/// Call `PyOS_FSPath` on `arg`.
///
/// Raises if the result is not a `str` or `str` subclass, as fathlib does not
/// support `bytes` paths. Returns a new reference.
PyUnicodeObject *_fspath(PyObject *arg);

#endif
