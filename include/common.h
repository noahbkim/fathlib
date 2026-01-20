#ifndef COMMON_H
#define COMMON_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

/// Call `PyOS_FSPath` on `arg`.
///
/// Raises if the result is not a `str` or `str` subclass, as fathlib does not
/// support `bytes` paths. Returns a new reference.
PyUnicodeObject *_fspath(PyObject *arg);

/// Join zero or more components into a string with a separator.
///
/// Essentially a rewrite of `PyUnicode_Join` that takes an array rather than a
/// tuple, permitting slicing of argument lists (e.g. to handle the presence of
/// root paths in joinpath calls).
PyUnicodeObject *_join(Py_UCS4 separator, PyUnicodeObject *const *args, Py_ssize_t nargs);

#endif
