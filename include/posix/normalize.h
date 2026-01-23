#ifndef POSIX_NORMALIZE_H
#define POSIX_NORMALIZE_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "cow.h"

typedef enum
{
    POSIX_NORMALIZE_STATE_START,
    POSIX_NORMALIZE_STATE_START_SLASH,
    POSIX_NORMALIZE_STATE_START_SLASH_SLASH,
    POSIX_NORMALIZE_STATE_START_SLASH_SLASH_SLASHES,
    POSIX_NORMALIZE_STATE_PART,
    POSIX_NORMALIZE_STATE_PART_SLASHES,
    POSIX_NORMALIZE_STATE_DOT,
    POSIX_NORMALIZE_STATE_DOT_SLASHES,
} PosixNormalizeState;

int _posix_normalize_impl(Cow *cow, PosixNormalizeState *state, PyUnicodeObject *read);
PyUnicodeObject *_posix_normalize(PyUnicodeObject *read);
PyObject *posix_normalize(PyObject *module, PyObject *read);

#endif
