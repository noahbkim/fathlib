#ifndef FATH_H
#define FATH_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

// MARK: Object

typedef struct
{
    // clang-format off
    PyObject_HEAD
    PyUnicodeObject *inner;
    // clang-format on
} PyFathObject;

// MARK: API

PyFathObject *PyFath_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);
int PyFath_init(PyFathObject *self, PyObject *args, PyObject *kwargs);
PyObject *PyFath_str(PyFathObject *self);
PyObject *PyFath_getstate(PyFathObject *self);
PyObject *PyFath_setstate(PyFathObject *self, PyObject *state);
void PyFath_dealloc(PyFathObject *self);

#endif
