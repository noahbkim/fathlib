#include "posix/view.h"

#include "common.h"
#include "posix/normalize.h"

// MARK: Is Absolute

int
_posix_is_absolute(PyUnicodeObject *arg)
{
    Py_ssize_t size = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);
    return size >= 1 && PyUnicode_READ(kind, data, 0) == '/';
}

PyObject *
posix_is_absolute(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    // We don't need to normalize so long as we check `size >= 1`.
    int is_absolute = _posix_is_absolute(fspath);
    Py_DECREF(fspath);
    if (is_absolute)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

// MARK: Root

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
    Py_DECREF(fspath);
    if (!normalized)
    {
        return NULL;
    }
    PyUnicodeObject *root = _posix_root(normalized);
    Py_DECREF(normalized);
    return (PyObject *)root;
}

// MARK: Name

PyUnicodeObject *
_posix_name(PyUnicodeObject *arg)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);

    // Read until the next slash or the start of the string.
    Py_ssize_t i = length - 1;
    while (i >= 0 && PyUnicode_READ(kind, data, i) != '/')
    {
        i -= 1;
    }

    // Optimization: use the same string if the whole thing is the name.
    if (i == -1)
    {
        Py_INCREF(arg);
        return arg;
    }
    else
    {
        Py_ssize_t start = i + 1;
        return (PyUnicodeObject *)PyUnicode_Substring((PyObject *)arg, start, length);
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
    Py_DECREF(fspath);
    if (!normalized)
    {
        return NULL;
    }
    PyUnicodeObject *name = _posix_name(normalized);
    Py_DECREF(normalized);
    return (PyObject *)name;
}

// MARK: Parent

Py_ssize_t
_posix_parent_index(PyUnicodeObject *arg)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);

    // Read until the next slash or the start of the string.
    Py_ssize_t i = length;
    while (i > 0 && PyUnicode_READ(kind, data, i - 1) != '/')
    {
        i -= 1;
    }

    return i;
}

PyObject *
posix_parent(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _posix_normalize(fspath);
    Py_DECREF(normalized);
    if (!normalized)
    {
        return NULL;
    }
    Py_ssize_t parent_index = _posix_parent_index(normalized);
    if (parent_index > 0)
    {
        return PyUnicode_Substring(arg, 0, parent_index);
    }
    else
    {
        Py_RETURN_NONE;
    }
}
