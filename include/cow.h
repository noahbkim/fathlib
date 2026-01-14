#ifndef COW_H
#define COW_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

/// A container that only allocates a copy of the referenced `PyUnicodeObject`
/// once a differing character is pushed. It does not support growing the
/// string beyond the size of `referenced`, and doing so will cause UB.
typedef struct
{
    PyUnicodeObject *referenced;
    void *referenced_data;
    PyUnicodeObject *owned;
    void *owned_data;
    Py_ssize_t cursor;
    unsigned int kind;
} Cow;

void cow_construct(Cow *self, PyUnicodeObject *referenced);
int cow_push(Cow *self, Py_UCS4 character);
PyUnicodeObject *cow_consume(Cow *self);

#endif
