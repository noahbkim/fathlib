#include "posix/view.h"

#include "common.h"
#include "posix/normalize.h"

PyUnicodeObject *
_posix_root(PyUnicodeObject *arg)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);
    if (length >= 1 && PyUnicode_READ(kind, data, 0) == '/')
    {
        if (length >= 2 && PyUnicode_READ(kind, data, 1) == '/')
        {
            if (length >= 3 && PyUnicode_READ(kind, data, 2) == '/')
            {
                return (PyUnicodeObject *)PyUnicode_FromString("/");
            }
            else
            {
                return (PyUnicodeObject *)PyUnicode_FromString("//");
            }
        }
        else
        {
            return (PyUnicodeObject *)PyUnicode_FromString("/");
        }
    }
    else
    {
        return (PyUnicodeObject *)PyUnicode_FromString("");
    }
}

PyObject *
posix_root(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _posix_normalize(fspath);
    if (!normalized)
    {
        return NULL;
    }
    PyUnicodeObject *root = _posix_root(normalized);
    Py_DECREF(normalized);
    return (PyObject *)root;
}

PyUnicodeObject *
_posix_name(PyUnicodeObject *arg)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);

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
        Py_INCREF(arg);
        return arg;
    }
    else
    {
        Py_ssize_t start = i + 1;
        return (PyUnicodeObject *)PyUnicode_Substring((PyObject *)arg, start, end);
    }
}

PyObject *
posix_name(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _posix_normalize(fspath);
    if (!normalized)
    {
        return NULL;
    }
    PyUnicodeObject *name = _posix_name(normalized);
    Py_DECREF(arg);
    return (PyObject *)name;
}
