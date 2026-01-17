#include "windows/join.h"
#include "common.h"

// MARK: POSIX

PyUnicodeObject *
_windows_join(PyObject *items, int count)
{
    PyObject *slash = PyUnicode_FromString("\\");
    if (!slash)
    {
        return NULL;
    }

    Py_ssize_t i;
    for (i = 0; i < count; ++i)
    {
        PyUnicodeObject *path = _fspath(PyTuple_GET_ITEM(items, i));
        if (!path)
        {
            goto error;
        }
        PyTuple_SET_ITEM(items, i, path);
    }

    PyObject *inner = PyUnicode_Join(slash, items);
    if (!inner)
    {
        goto error;
    }

    return (PyUnicodeObject *)inner;

error:
    Py_DECREF(slash);
    for (Py_ssize_t j = 0; j < i; ++j)
    {
        Py_DECREF(PyTuple_GET_ITEM(items, j));
    }

    return NULL;
}

PyObject *
windows_join(PyObject *module, PyObject *items)
{
    Py_ssize_t count = PyTuple_GET_SIZE(items);
    if (count == 0)
    {
        return PyUnicode_FromStringAndSize(".", 1);
    }
    else
    {
        return (PyObject *)_windows_join(items, count);
    }
}
