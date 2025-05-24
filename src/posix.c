#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "posix.h"

PyObject *PyPosixFath(PyUnicodeObject *inner)
{
    PyPosixFathObject *self = PyObject_New(PyPosixFathObject, &PyPosixFath_Type);
    if (!self)
    {
        Py_DECREF(inner);
        return NULL;
    }

    self->inner = inner;
    return (PyObject *)self;
}

// MARK: Intrinsic

PyObject *PyPosixFath_repr(PyPosixFathObject *self)
{
    PyObject *inner = PyUnicode_Type.tp_repr((PyObject *)self->inner);
    PyObject *repr = PyUnicode_FromFormat("PosixFath(%U)", inner);
    Py_DECREF(inner);
    return repr;
}

Py_hash_t PyPosixFath_hash(PyPosixFathObject *self)
{
    // TODO: figure out if this is a reasonable trick.
    return PyUnicode_Type.tp_hash((PyObject *)self->inner) + 1;
}

PyObject *PyPosixFath_richcompare(PyPosixFathObject *self, PyObject *other, int op)
{
    if (PyPosixFath_Check(other))
    {
        PyObject *left = (PyObject *)self->inner;
        PyObject *right = (PyObject *)((PyPosixFathObject *)other)->inner;
        return PyUnicode_Type.tp_richcompare(left, right, op);
    }
    else
    {
        return Py_NotImplemented;
    }
}

// MARK: Methods

PyObject *PyPosixFath_name(PyPosixFathObject *self)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(self->inner);
    int kind = PyUnicode_KIND(self->inner);
    void *data = PyUnicode_DATA(self->inner);

    // Skip trailing slashes
    Py_ssize_t i = length - 1;
    while (i >= 0 && PyUnicode_READ(kind, data, i) == '/')
    {
        i -= 1;
    }

    Py_ssize_t end = i + 1;

    // Read until the next slash or the start of the string.
    while (i >= 0 && PyUnicode_READ(kind, data, i) != '/')
    {
        i -= 1;
    }

    // Optimization: use the same string if the whole thing is the name.
    if (i < 0 && end == length)
    {
        return Py_NewRef(self->inner);
    }
    else
    {
        Py_ssize_t start = i + 1;
        return PyUnicode_Substring((PyObject *)self->inner, start, end);
    }
}

PyObject *PyPosixFath_parent(PyPosixFathObject *self)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(self->inner);
    int kind = PyUnicode_KIND(self->inner);
    void *data = PyUnicode_DATA(self->inner);

    // Skip trailing slashes
    Py_ssize_t i = length - 1;
    while (i >= 0 && PyUnicode_READ(kind, data, i) == '/')
    {
        i -= 1;
    }

    // Read until the next slash or the start of the string.
    while (i >= 0 && PyUnicode_READ(kind, data, i) != '/')
    {
        i -= 1;
    }

    if (i > 0)
    {
        PyObject *parent = PyUnicode_Substring((PyObject *)self->inner, 0, i);
        return PyPosixFath((PyUnicodeObject *)parent);
    }
    else
    {
        Py_RETURN_NONE;
    }
}

// MARK: Declaration

static PyMethodDef PyPosixFath_methods[] = {
    {"__getstate__", (PyCFunction)PyFath_getstate, METH_NOARGS, PyDoc_STR("Serialize this fath for pickling")},
    {"__setstate__", (PyCFunction)PyFath_setstate, METH_O, PyDoc_STR("Deserialize this fath for pickling")},
    {NULL, NULL, 0, NULL},
};

static PyGetSetDef PyPosixFath_getset[] = {
    {"name", (getter)PyPosixFath_name, NULL, PyDoc_STR("Get the base name of the fath"), NULL},
    {"parent", (getter)PyPosixFath_parent, NULL, PyDoc_STR("Get the parent fath"), NULL},
    {NULL, NULL, NULL, NULL, NULL},
};

PyTypeObject PyPosixFath_Type = {
    // clang-format off
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "fathlib.PosixFath",
    // clang-format on
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = PyDoc_STR("A faster alternative to fathlib.Fath"),
    .tp_basicsize = sizeof(PyPosixFathObject),
    .tp_itemsize = 0,
    .tp_new = (newfunc)PyFath_new,
    .tp_init = (initproc)PyFath_init,
    .tp_hash = (hashfunc)PyPosixFath_hash,
    .tp_richcompare = (richcmpfunc)PyPosixFath_richcompare,
    .tp_repr = (reprfunc)PyPosixFath_repr,
    .tp_str = (reprfunc)PyFath_str,
    .tp_dealloc = (destructor)PyFath_dealloc,
    .tp_methods = PyPosixFath_methods,
    .tp_getset = PyPosixFath_getset,
};
