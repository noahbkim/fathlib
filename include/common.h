#ifndef COMMON_H
#define COMMON_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *_fspath(PyObject *item);

int _cow_copy(PyUnicodeObject *read,
              Py_ssize_t read_size,
              unsigned int read_kind,
              void *read_data,
              PyUnicodeObject **write,
              void **write_data);
PyUnicodeObject *_cow_consume(PyUnicodeObject *read,
                              Py_ssize_t read_size,
                              unsigned int read_kind,
                              void *read_data,
                              PyUnicodeObject *write,
                              Py_ssize_t write_index);

#endif
