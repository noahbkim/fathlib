#ifndef COW_H
#define COW_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

typedef struct
{
    PyUnicodeObject *read;
    PyUnicodeObject *write;
    Py_ssize_t read_size;
    Py_ssize_t write_size;
    void *read_data;
    void *write_data;
    Py_ssize_t read_index;
    Py_ssize_t write_index;
    unsigned int read_kind;
    unsigned int write_kind;
} Cow;

void cow_construct(Cow *self, PyUnicodeObject *read);
int cow_write(Cow *self, Py_UCS4 character);
PyUnicodeObject *cow_consume(Cow *self);

#endif
