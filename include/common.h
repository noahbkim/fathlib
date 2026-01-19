#ifndef COMMON_H
#define COMMON_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

/// Call `PyOS_FSPath` on `arg`.
///
/// Raises if the result is not a `str` or `str` subclass, as fathlib does not
/// support `bytes` paths. Returns a new reference.
PyUnicodeObject *_fspath(PyObject *arg);

/// Copy the contents of the `read` string into `write`.
///
/// Does nothing if `*write` has already been assigned. Assigns `*write` to
/// `read` if the latter has only one reference.
int _cow_copy(PyUnicodeObject *read,
              Py_ssize_t read_size,
              unsigned int read_kind,
              void *read_data,
              PyUnicodeObject **write,
              Py_ssize_t write_size,
              void **write_data);

/// Consume a `read` and `write` string into a single `PyUnicodeObject`.
///
/// Handles permutations of `read`, `write`, and `write_size` that arise while
/// normalizing paths:
///
///   1. `write` is present: trim `write` to `write_size` length and return
///      it after calling `Py_DECREF` on `read`.
///   2. `write` is `NULL` and `write_size != read_size`: no copy was created,
///      but we need to remove characters from the end of the string, so create
///      a copy of `read` with size `write_index`, returning it after calling
///      `Py_DECREF` on `read`.
///   3. `write` is `NULL` and `write_size == read_size`: no copy was created
///      and we can reuse the original string, so just return it.
///
/// The `read` reference is borrowed and the `write` reference is consumed.
/// Returns a new reference to the resulting `PyUnicodeObject`.
PyUnicodeObject *_cow_consume(PyUnicodeObject *read,
                              Py_ssize_t read_size,
                              unsigned int read_kind,
                              void *read_data,
                              PyUnicodeObject *write,
                              Py_ssize_t write_size);

#endif
