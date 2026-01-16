#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "windows/fath.h"

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
        self->inner = (PyUnicodeObject *)PyUnicode_New(0, 0);
        return 0;
    }

    // Call `__fspath__` on a single argument
    else if (nargs == 1)
    {
        PyObject *arg = PyTuple_GET_ITEM(args, 0);
        PyObject *fspath = PyOS_FSPath(arg);
        if (!fspath)
        {
            goto error;
        }
        if (PyBytes_CheckExact(fspath))
        {
            PyErr_Format(PyExc_TypeError, "fathlib.Fath does not support bytes faths");
            Py_DECREF(fspath);
            goto error;
        }
        if (!PyUnicode_CheckExact(fspath))
        {
            PyErr_Format(PyExc_TypeError, "fathlib.Fath cannot be constructed from %T", fspath);
            Py_DECREF(fspath);
            goto error;
        }

        self->inner = (PyUnicodeObject *)fspath;
        return 0;
    }

    // Join the `__fspath__` of multiple arguments.
    if (nargs > 1)
    {
        assert(Py_REFCNT(args) == 1);
        for (Py_ssize_t i = 0; i < nargs; ++i)
        {
            PyObject *fspath = PyOS_FSPath(PyTuple_GET_ITEM(args, i));
            if (!fspath)
            {
                goto error;
            }
            if (PyBytes_CheckExact(fspath))
            {
                PyErr_Format(PyExc_TypeError, "fathlib.WindowsFath does not support bytes faths");
                Py_DECREF(fspath);
                goto error;
            }
            if (!PyUnicode_CheckExact(fspath))
            {
                PyErr_Format(PyExc_TypeError, "fathlib.WindowsFath cannot be constructed from %T", fspath);
                Py_DECREF(fspath);
                goto error;
            }
            Py_DECREF(PyTuple_GET_ITEM(args, i));
            PyTuple_SET_ITEM(args, i, fspath);
        }

        PyObject *slash = PyUnicode_FromString("\\");
        if (!slash)
        {
            goto error;
        }

        PyObject *inner = PyUnicode_Join(slash, args);
        self->inner = (PyUnicodeObject *)inner;
        return 0;
    }

error:
    Py_XDECREF(args);
    Py_XDECREF(kwargs);
    return -1;
}

PyObject *
PyWindowsFath_repr(PyWindowsFathObject *self)
{
    PyObject *inner = PyUnicode_Type.tp_repr((PyObject *)self->inner);
    PyObject *cls = PyObject_Type((PyObject *)self);
    PyObject *cls_name = PyType_GetName((PyTypeObject *)cls);
    PyObject *repr = PyUnicode_FromFormat("%U(%U)", cls_name, inner);
    Py_DECREF(inner);
    Py_DECREF(cls);
    Py_DECREF(cls_name);
    return repr;
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
        return Py_NotImplemented;
    }
}

// MARK: Methods

Py_UCS4
PyWindowsFath_last(PyWindowsFathObject *self)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(self->inner);
    int kind = PyUnicode_KIND(self->inner);
    void *data = PyUnicode_DATA(self->inner);
    return PyUnicode_READ(kind, data, length - 1);
}

PyObject *
PyWindowsFath_name(PyWindowsFathObject *self)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(self->inner);
    int kind = PyUnicode_KIND(self->inner);
    void *data = PyUnicode_DATA(self->inner);

    // Skip trailing slashes
    Py_ssize_t i = length - 1;
    while (i >= 0 && PyUnicode_READ(kind, data, i) == '\\')
    {
        i -= 1;
    }

    Py_ssize_t end = i + 1;

    // Read until the next slash or the start of the string.
    while (i >= 0 && PyUnicode_READ(kind, data, i) != '\\')
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

PyObject *
PyWindowsFath_parent(PyWindowsFathObject *self)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(self->inner);
    int kind = PyUnicode_KIND(self->inner);
    void *data = PyUnicode_DATA(self->inner);

    // Skip trailing slashes
    Py_ssize_t i = length - 1;
    while (i >= 0 && PyUnicode_READ(kind, data, i) == '\\')
    {
        i -= 1;
    }

    // Read until the next slash or the start of the string.
    while (i >= 0 && PyUnicode_READ(kind, data, i) != '\\')
    {
        i -= 1;
    }

    if (i > 0)
    {
        PyObject *parent_inner = PyUnicode_Substring((PyObject *)self->inner, 0, i);
        PyObject *cls = PyObject_Type((PyObject *)self);
        if (!cls)
        {
            return NULL;
        }

        return PyObject_CallOneArg(cls, parent_inner);
    }
    else
    {
        Py_RETURN_NONE;
    }
}

PyObject *
PyWindowsFath_as_posix(PyWindowsFathObject *self)
{
    PyObject *find = NULL;
    PyObject *replace = NULL;
    PyObject *result = NULL;

    find = PyUnicode_FromString("/");
    if (!find)
    {
        goto error;
    }

    replace = PyUnicode_FromString("\\");
    if (!replace)
    {
        goto error;
    }

    result = PyUnicode_Replace((PyObject *)self->inner, find, replace, -1);
    if (!result)
    {
        goto error;
    }

    goto done;

error:
    Py_XDECREF(find);
    Py_XDECREF(replace);

done:
    return result;
}

PyObject *
PyWindowsFath_joinpath(PyObject *head, PyObject *tail)
{
    if (!PyWindowsFath_Check(head))
    {
        Py_RETURN_NOTIMPLEMENTED;
    }

    PyWindowsFathObject *self = (PyWindowsFathObject *)head;
    PyObject *tail_inner;
    PyObject *joined_inner = NULL;
    PyObject *joined = NULL;

    tail_inner = PyOS_FSPath(tail);
    if (!tail_inner)
    {
        return NULL;
    }

    const char *format = PyWindowsFath_last(self) == '\\' ? "%U%U" : "%U\\%U";
    joined_inner = PyUnicode_FromFormat(format, self->inner, tail_inner);
    if (!joined_inner)
    {
        goto error;
    }

    PyObject *cls = PyObject_Type((PyObject *)self);
    if (!cls)
    {
        return NULL;
    }

    joined = PyObject_CallOneArg(cls, joined_inner);
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
    {"as_posix",     (PyCFunction)PyWindowsFath_as_posix, METH_NOARGS, PyDoc_STR("Normalize the path with posix slashes")},
    {"joinpath",     (PyCFunction)PyWindowsFath_joinpath, METH_O,      PyDoc_STR("Append another path")                  },
    {"__getstate__", (PyCFunction)PyFath_getstate,        METH_NOARGS, PyDoc_STR("Serialize this fath for pickling")     },
    {"__setstate__", (PyCFunction)PyFath_setstate,        METH_O,      PyDoc_STR("Deserialize this fath for pickling")   },
    {NULL,           NULL,                                0,           NULL                                              },
};

static PyGetSetDef PyWindowsFath_getset[] = {
    {"name",   (getter)PyWindowsFath_name,   NULL, PyDoc_STR("Get the base name of the fath"), NULL},
    {"parent", (getter)PyWindowsFath_parent, NULL, PyDoc_STR("Get the parent fath"),           NULL},
    {NULL,     NULL,                         NULL, NULL,                                       NULL},
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
    .tp_init = (initproc)PyWindowsFath_init,
    .tp_hash = (hashfunc)PyWindowsFath_hash,
    .tp_richcompare = (richcmpfunc)PyWindowsFath_richcompare,
    .tp_repr = (reprfunc)PyWindowsFath_repr,
    .tp_str = (reprfunc)PyFath_str,
    .tp_dealloc = (destructor)PyFath_dealloc,
    .tp_as_number = &PyWindowsFath_as_number,
    .tp_methods = PyWindowsFath_methods,
    .tp_getset = PyWindowsFath_getset,
};
