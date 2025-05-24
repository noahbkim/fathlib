#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "generic.h"

// MARK: Intrinsic

PyFathObject *PyFath_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
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

int PyFath_init(PyFathObject *self, PyObject *args, PyObject *kwargs)
{
    if (kwargs)
    {
        PyErr_Format(PyExc_TypeError,
                     "fathlib.Fath takes no keyword arguments");
        return -1;
    }

    // Default constructor
    Py_ssize_t nargs = PyTuple_GET_SIZE(args);
    if (nargs == 0)
    {
        self->inner = (PyUnicodeObject *)PyUnicode_New(0, 0);
        return 0;
    }

    if (nargs > 1)
    {
        PyErr_Format(PyExc_TypeError,
                     "fathlib.Fath expected 1 argument, got %zd", nargs);
        return -1;
    }

    // `__fsfath__` constructor
    PyObject *arg = PyTuple_GET_ITEM(args, 0);
    PyObject *fspath = PyOS_FSPath(arg);
    if (!fspath)
    {
        return -1;
    }
    if (PyBytes_CheckExact(fspath))
    {
        PyErr_Format(PyExc_TypeError,
                     "fathlib.Fath does not support bytes faths");
        Py_DECREF(fspath);
        return -1;
    }
    if (!PyUnicode_CheckExact(fspath))
    {
        PyErr_Format(PyExc_TypeError,
                     "fathlib.Fath cannot be constructed from %T", fspath);
        Py_DECREF(fspath);
        return -1;
    }

    self->inner = (PyUnicodeObject *)fspath;
    return 0;
}

PyObject *PyFath_str(PyFathObject *self)
{
    Py_INCREF(self->inner);
    return (PyObject *)self->inner;
}

PyObject *PyFath_getstate(PyFathObject *self)
{
    Py_INCREF(self->inner);
    return (PyObject *)self->inner;
}

PyObject *PyFath_setstate(PyFathObject *self, PyObject *state)
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

void PyFath_dealloc(PyFathObject *self)
{
    Py_DECREF(self->inner);
    Py_TYPE(self)->tp_free((PyObject *)self);
}
