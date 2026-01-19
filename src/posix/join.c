#include "posix/join.h"
#include "common.h"

// MARK: POSIX

PyUnicodeObject *
_posix_join(PyObject *items, int count)
{
    PyObject *joined = NULL;

    PyObject *slash = PyUnicode_FromString("/");
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
        Py_DECREF(PyTuple_GET_ITEM(items, i));
        PyTuple_SET_ITEM(items, i, path);
    }

    joined = PyUnicode_Join(slash, items);

error:
    Py_DECREF(slash);

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
