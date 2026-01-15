#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "generic.h"

// MARK: Intrinsic

PyFathObject *
PyFath_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    PyFathObject *self = (PyFathObject *)type->tp_alloc(type, 0);
    if (!self)
    {
        return NULL;
    }

    self->inner = (PyUnicodeObject *)PyUnicode_New(0, 0);
    if (!self->inner)
    {
        Py_DECREF(self);
        return NULL;
    }

    return self;
}

PyObject *
PyFath_str(PyFathObject *self)
{
    Py_INCREF(self->inner);
    return (PyObject *)self->inner;
}

PyObject *
PyFath_getstate(PyFathObject *self)
{
    Py_INCREF(self->inner);
    return (PyObject *)self->inner;
}

PyObject *
PyFath_setstate(PyFathObject *self, PyObject *state)
{
    if (PyUnicode_Check(state))
    {
        self->inner = (PyUnicodeObject *)state;
        Py_RETURN_NONE;
    }
    else
    {
        PyErr_Format(PyExc_TypeError, "Invalid %T state %R", self, state);
        return NULL;
    }
}

void
PyFath_dealloc(PyFathObject *self)
{
    Py_DECREF(self->inner);
    Py_TYPE(self)->tp_free((PyObject *)self);
}
