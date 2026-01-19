#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "common.h"
#include "windows/fath.h"
#include "windows/join.h"
#include "windows/normalize.h"
#include "windows/view.h"

// MARK: Intrinsic

int
PyWindowsFath_init(PyFathObject *self, PyObject *args, PyObject *kwargs)
{
    if (kwargs)
    {
        PyErr_Format(PyExc_TypeError, "fathlib.WindowsFath takes no keyword arguments");
        return -1;
    }

    // Default constructor
    Py_ssize_t nargs = PyTuple_GET_SIZE(args);
    if (nargs == 0)
    {
        self->inner = (PyUnicodeObject *)PyUnicode_FromString(".");
        return 0;
    }

    // Call `__fspath__` on a single argument
    else if (nargs == 1)
    {
        PyObject *arg = PyTuple_GET_ITEM(args, 0);

        PyUnicodeObject *path = _fspath(arg);
        if (!path)
        {
            return -1;
        }

        PyUnicodeObject *normalized = _windows_normalize(path);
        if (!normalized)
        {
            return -1;
        }

        self->inner = normalized;
        return 0;
    }

    // Join the `__fspath__` of multiple arguments.
    else
    {
        PyUnicodeObject *joined = _windows_join(args, nargs);
        if (!joined)
        {
            return -1;
        }

        PyUnicodeObject *normalized = _windows_normalize(joined);
        if (!normalized)
        {
            return -1;
        }

        self->inner = normalized;
        return 0;
    }
}

Py_hash_t
PyWindowsFath_hash(PyWindowsFathObject *self)
{
    // TODO: figure out if this is a reasonable trick.
    return PyUnicode_Type.tp_hash((PyObject *)self->inner) + 1;
}

PyObject *
PyWindowsFath_richcompare(PyWindowsFathObject *self, PyObject *other, int op)
{
    if (PyWindowsFath_Check(other))
    {
        PyObject *left = (PyObject *)self->inner;
        PyObject *right = (PyObject *)((PyWindowsFathObject *)other)->inner;
        return PyUnicode_Type.tp_richcompare(left, right, op);
    }
    else
    {
        Py_RETURN_NOTIMPLEMENTED;
    }
}

// MARK: Methods

PyObject *
PyWindowsFath_as_posix(PyWindowsFathObject *self)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(self->inner);
    unsigned int kind = PyUnicode_KIND(self->inner);
    void *data = PyUnicode_DATA(self->inner);
    PyObject *posix = PyUnicode_FromKindAndData(kind, data, length);

    data = PyUnicode_DATA(posix);
    for (Py_ssize_t i = 0; i < length; ++i)
    {
        if (PyUnicode_READ(kind, data, i) == '\\')
        {
            PyUnicode_WRITE(kind, data, i, '/');
        }
    }

    return posix;
}

PyObject *
PyWindowsFath_joinpath(PyWindowsFathObject *self, PyObject *args)
{
    Py_ssize_t count = PyTuple_GET_SIZE(args);
    PyObject *concat = PyTuple_New(count + 1);
    if (!concat)
    {
        return NULL;
    }

    Py_INCREF(self->inner);
    PyTuple_SET_ITEM(concat, 0, self->inner);
    for (Py_ssize_t i = 0; i < count; ++i)
    {
        PyObject *arg = PyTuple_GET_ITEM(args, i);
        Py_INCREF(arg);
        PyTuple_SET_ITEM(concat, i + 1, arg);
    }

    PyObject *cls = PyObject_Type((PyObject *)self);
    if (!cls)
    {
        return NULL;
    }

    PyObject *joined = PyObject_Call(cls, concat, NULL);
    Py_DECREF(cls);
    Py_DECREF(concat);
    if (!joined)
    {
        return NULL;
    }

    return joined;
}

// MARK: Properties

PyObject *
PyWindowsFath_drive(PyWindowsFathObject *self)
{
    return (PyObject *)_windows_drive(self->inner);
}

PyObject *
PyWindowsFath_root(PyWindowsFathObject *self)
{
    return (PyObject *)_windows_root(self->inner);
}

PyObject *
PyWindowsFath_name(PyWindowsFathObject *self)
{
    return (PyObject *)_windows_name(self->inner);
}

PyObject *
PyWindowsFath_parent(PyWindowsFathObject *self)
{
    Py_ssize_t parent_index = _windows_parent_index(self->inner);
    if (parent_index > 0)
    {
        PyObject *inner = PyUnicode_Substring((PyObject *)self->inner, 0, parent_index);
        if (!inner)
        {
            return NULL;
        }

        PyObject *cls = PyObject_Type((PyObject *)self);
        if (!cls)
        {
            Py_DECREF(inner);
            return NULL;
        }

        PyObject *parent = PyObject_CallOneArg(cls, inner);
        Py_DECREF(cls);
        Py_DECREF(inner);
        return parent;
    }
    else
    {
        Py_RETURN_NONE;
    }
}

// MARK: Operators

PyObject *
PyWindowsFath_true_divide(PyObject *self, PyObject *arg)
{
    if (!PyWindowsFath_Check(self))
    {
        Py_RETURN_NOTIMPLEMENTED;
    }

    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }

    PyObject *inner = PyUnicode_FromFormat("%U\\%U", ((PyWindowsFathObject *)self)->inner, fspath);
    Py_DECREF(fspath);
    if (!inner)
    {
        return NULL;
    }

    PyObject *cls = PyObject_Type((PyObject *)self);
    if (!cls)
    {
        Py_DECREF(inner);
        return NULL;
    }

    PyObject *joined = PyObject_CallOneArg(cls, inner);
    Py_DECREF(cls);
    Py_DECREF(inner);
    return joined;
}

// MARK: Declaration

static PyMethodDef PyWindowsFath_methods[] = {
    {"as_posix",     (PyCFunction)PyWindowsFath_as_posix, METH_NOARGS,  PyDoc_STR("Normalize the path with posix slashes")},
    {"joinpath",     (PyCFunction)PyWindowsFath_joinpath, METH_VARARGS, PyDoc_STR("Append another path")                  },
    {"__fspath__",   (PyCFunction)PyFath_str,             METH_NOARGS,  PyDoc_STR("Get the underlying string")            },
    {"__getstate__", (PyCFunction)PyFath_getstate,        METH_NOARGS,  PyDoc_STR("Serialize this fath for pickling")     },
    {"__setstate__", (PyCFunction)PyFath_setstate,        METH_O,       PyDoc_STR("Deserialize this fath for pickling")   },
    {NULL,           NULL,                                0,            NULL                                              },
};

static PyGetSetDef PyWindowsFath_getset[] = {
    {"drive",  (getter)PyWindowsFath_drive,  NULL, PyDoc_STR("Get the drive of the fath"),     NULL},
    {"root",   (getter)PyWindowsFath_root,   NULL, PyDoc_STR("Get the root of the fath"),      NULL},
    {"name",   (getter)PyWindowsFath_name,   NULL, PyDoc_STR("Get the base name of the fath"), NULL},
    {"parent", (getter)PyWindowsFath_parent, NULL, PyDoc_STR("Get the parent fath"),           NULL},
    {NULL,     NULL,                         NULL, NULL,                                       NULL},
};

static PyNumberMethods PyWindowsFath_as_number = {
    .nb_true_divide = (PyCFunction)PyWindowsFath_true_divide,
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
    .tp_init = (initproc)PyWindowsFath_init,
    .tp_hash = (hashfunc)PyWindowsFath_hash,
    .tp_richcompare = (richcmpfunc)PyWindowsFath_richcompare,
    .tp_repr = (reprfunc)PyFath_repr,
    .tp_str = (reprfunc)PyFath_str,
    .tp_dealloc = (destructor)PyFath_dealloc,
    .tp_as_number = &PyWindowsFath_as_number,
    .tp_methods = PyWindowsFath_methods,
    .tp_getset = PyWindowsFath_getset,
};
