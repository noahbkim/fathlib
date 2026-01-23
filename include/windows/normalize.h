#ifndef WINDOWS_NORMALIZE_H
#define WINDOWS_NORMALIZE_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "cow.h"

typedef enum
{
    WINDOWS_NORMALIZE_STATE_START,
    WINDOWS_NORMALIZE_STATE_START_SLASHES,
    WINDOWS_NORMALIZE_STATE_PART,
    WINDOWS_NORMALIZE_STATE_PART_SLASHES,
    WINDOWS_NORMALIZE_STATE_DOT,
    WINDOWS_NORMALIZE_STATE_DOT_SLASHES,
} WindowsNormalizeState;

int _windows_normalize_impl(Cow *cow,
                            WindowsNormalizeState *state,
                            Py_ssize_t read_size,
                            unsigned int read_kind,
                            void *read_data,
                            Py_ssize_t read_index);
PyUnicodeObject *_windows_normalize(PyUnicodeObject *read);
PyObject *windows_normalize(PyObject *module, PyObject *read);

#endif
