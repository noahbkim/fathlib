#include "posix/join.h"
#include "common.h"

// MARK: POSIX

PyUnicodeObject *
_posix_join(PyObject *items, int count)
{
    PyObject *joined = NULL;
    PyObject *slash = NULL;
    PyObject *fspaths = NULL;

    slash = PyUnicode_FromString("/");
    if (!slash)
    {
        goto error;
    }

    fspaths = PyTuple_New(count);
    if (!fspaths)
    {
        goto error;
    }

    for (Py_ssize_t i = 0; i < count; ++i)
    {
        PyUnicodeObject *fspath = _fspath(PyTuple_GET_ITEM(items, i));
        if (!fspath)
        {
            goto error;
        }
        PyTuple_SET_ITEM(fspaths, i, fspath);
    }

    joined = PyUnicode_Join(slash, fspaths);

error:
    Py_DECREF(slash);
    Py_XDECREF(fspaths);

    return (PyUnicodeObject *)joined;
}

PyObject *
posix_join(PyObject *module, PyObject *items)
{
    Py_ssize_t count = PyTuple_GET_SIZE(items);
    if (count == 0)
    {
        return PyUnicode_FromStringAndSize(".", 1);
    }
    else
    {
        return (PyObject *)_posix_join(items, count);
    }
}
