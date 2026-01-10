#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "windows.h"

PyObject *PyWindowsFath(PyUnicodeObject *inner)
{
    PyWindowsFathObject *self = PyObject_New(PyWindowsFathObject, &PyWindowsFath_Type);
    if (!self)
    {
        Py_DECREF(inner);
        return NULL;
    }

    self->inner = inner;
    return (PyObject *)self;
}

// MARK: Intrinsic

PyObject *PyWindowsFath_repr(PyWindowsFathObject *self)
{
    PyObject *inner = PyUnicode_Type.tp_repr((PyObject *)self->inner);
    PyObject *repr = PyUnicode_FromFormat("WindowsFath(%U)", inner);
    Py_DECREF(inner);
    return repr;
}

Py_hash_t PyWindowsFath_hash(PyWindowsFathObject *self)
{
    // TODO: figure out if this is a reasonable trick.
    return PyUnicode_Type.tp_hash((PyObject *)self->inner) + 1;
}

PyObject *PyWindowsFath_richcompare(PyWindowsFathObject *self, PyObject *other, int op)
{
    if (PyWindowsFath_Check(other))
    {
        PyObject *left = (PyObject *)self->inner;
        PyObject *right = (PyObject *)((PyWindowsFathObject *)other)->inner;
        return PyUnicode_Type.tp_richcompare(left, right, op);
    }
    else
    {
        return Py_NotImplemented;
    }
}

// MARK: Methods

Py_UCS4 PyWindowsFath_last(PyWindowsFathObject *self)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(self->inner);
    int kind = PyUnicode_KIND(self->inner);
    void *data = PyUnicode_DATA(self->inner);
    return PyUnicode_READ(kind, data, length - 1);
}

PyObject *PyWindowsFath_name(PyWindowsFathObject *self)
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

PyObject *PyWindowsFath_parent(PyWindowsFathObject *self)
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
        return PyWindowsFath((PyUnicodeObject *)parent);
    }
    else
    {
        Py_RETURN_NONE;
    }
}

PyObject *PyWindowsFath_joinpath(PyObject *head, PyObject *tail)
{
    if (!PyWindowsFath_CheckExact(head))
    {
        Py_RETURN_NOTIMPLEMENTED;
    }

    PyWindowsFathObject *self = (PyWindowsFathObject *)head;
    PyObject *tail_inner;
    PyObject *joined_inner = NULL;
    PyObject *joined = NULL;

    if (PyWindowsFath_CheckExact(tail))
    {
        tail_inner = (PyObject *)((PyWindowsFathObject *)tail)->inner;
        Py_INCREF(tail_inner);
    }
    else
    {
        tail_inner = PyOS_FSPath(tail);
        if (!tail_inner)
        {
            return NULL;
        }
    }

    const char *format = PyWindowsFath_last(self) == '/' ? "%U%U" : "%U/%U";
    joined_inner = PyUnicode_FromFormat(format, self->inner, tail_inner);
    if (!joined_inner)
    {
        goto error;
    }

    joined = PyWindowsFath((PyUnicodeObject *)joined_inner);
    if (!joined)
    {
        goto error;
    }

    goto done;

error:
    Py_XDECREF(joined_inner);
    Py_XDECREF(joined);

done:
    Py_XDECREF(tail_inner);
    return joined;
}

// MARK: Declaration

static PyMethodDef PyWindowsFath_methods[] = {
    {"joinpath", (PyCFunction)PyWindowsFath_joinpath, METH_O, PyDoc_STR("Append another path")},
    {"__getstate__", (PyCFunction)PyFath_getstate, METH_NOARGS, PyDoc_STR("Serialize this fath for pickling")},
    {"__setstate__", (PyCFunction)PyFath_setstate, METH_O, PyDoc_STR("Deserialize this fath for pickling")},
    {NULL, NULL, 0, NULL},
};

static PyGetSetDef PyWindowsFath_getset[] = {
    {"name", (getter)PyWindowsFath_name, NULL, PyDoc_STR("Get the base name of the fath"), NULL},
    {"parent", (getter)PyWindowsFath_parent, NULL, PyDoc_STR("Get the parent fath"), NULL},
    {NULL, NULL, NULL, NULL, NULL},
};

static PyNumberMethods PyWindowsFath_as_number = {
    .nb_true_divide = (PyCFunction)PyWindowsFath_joinpath,
};

PyTypeObject PyWindowsFath_Type = {
    // clang-format off
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "fathlib.WindowsFath",
    // clang-format on
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = PyDoc_STR("A faster alternative to pathlib.Path"),
    .tp_basicsize = sizeof(PyWindowsFathObject),
    .tp_itemsize = 0,
    .tp_new = (newfunc)PyFath_new,
    .tp_init = (initproc)PyFath_init,
    .tp_hash = (hashfunc)PyWindowsFath_hash,
    .tp_richcompare = (richcmpfunc)PyWindowsFath_richcompare,
    .tp_repr = (reprfunc)PyWindowsFath_repr,
    .tp_str = (reprfunc)PyFath_str,
    .tp_dealloc = (destructor)PyFath_dealloc,
    .tp_as_number = &PyWindowsFath_as_number,
    .tp_methods = PyWindowsFath_methods,
    .tp_getset = PyWindowsFath_getset,
};
