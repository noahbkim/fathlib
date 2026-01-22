#include "windows/view.h"

#include "common.h"
#include "cow.h"
#include "windows/drive.h"
#include "windows/normalize.h"

#define COW_ADVANCE(SELF, READ)                                                                                        \
    if (cow_advance((SELF), (READ)) != 0)                                                                              \
    {                                                                                                                  \
        goto error;                                                                                                    \
    }

// MARK: Is Absolute

int
_windows_is_absolute(PyUnicodeObject *arg)
{
    Py_ssize_t size = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);
    if (size >= 1)
    {
        Py_UCS4 first = PyUnicode_READ(kind, data, 0);
        return first == '\\' || first == '/';
    }
    if (size >= 3 && PyUnicode_READ(kind, data, 1) == ':')
    {
        Py_UCS4 third = PyUnicode_READ(kind, data, 2);
        return third == '\\' || third == '/';
    }
    return 0;
}

PyObject *
windows_is_absolute(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    // We don't need to normalize so long as we check `size >= 1`.
    int is_absolute = _windows_is_absolute(fspath);
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

// MARK: As POSIX

PyUnicodeObject *
_windows_as_posix(PyUnicodeObject *read)
{
    Cow cow;
    cow_construct(&cow, read);

    for (Py_ssize_t read_index = 0; read_index < cow.read_size; ++read_index)
    {
        Py_UCS4 character = PyUnicode_READ(cow.read_kind, cow.read_data, read_index);
        if (character == '\\')
        {
            COW_ADVANCE(&cow, '/');
        }
        else
        {
            COW_ADVANCE(&cow, character);
        }
    }

    return cow_consume(&cow);

error:
    cow_destroy(&cow);
    return NULL;
}

PyObject *
windows_as_posix(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _windows_normalize(fspath);
    Py_DECREF(fspath);
    if (!normalized)
    {
        return NULL;
    }
    PyUnicodeObject *as_posix = _windows_as_posix(normalized);
    Py_DECREF(normalized);
    return (PyObject *)as_posix;
}

// MARK: Drive

PyUnicodeObject *
_windows_drive(PyUnicodeObject *read)
{
    Py_ssize_t read_size = PyUnicode_GET_LENGTH(read);
    int read_kind = PyUnicode_KIND(read);
    void *read_data = PyUnicode_DATA(read);

    Py_ssize_t unc_index = _windows_unc_index_impl(read_size, read_kind, read_data);
    if (unc_index != 0)
    {
        return (PyUnicodeObject *)PyUnicode_Substring((PyObject *)read, 0, unc_index);
    }

    if (read_size >= 3 && PyUnicode_READ(read_kind, read_data, 1) == ':')
    {
        return (PyUnicodeObject *)PyUnicode_Substring((PyObject *)read, 0, 2);
    }
    else
    {
        return (PyUnicodeObject *)PyUnicode_FromString("");
    }
}

PyObject *
windows_drive(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _windows_normalize(fspath);
    Py_DECREF(fspath);
    if (!normalized)
    {
        return NULL;
    }
    PyUnicodeObject *drive = _windows_drive(normalized);
    Py_DECREF(normalized);
    return (PyObject *)drive;
}

// MARK: Root

PyUnicodeObject *
_windows_root(PyUnicodeObject *arg)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);
    if (length > 0 && PyUnicode_READ(kind, data, 0) == '\\')
    {
        return (PyUnicodeObject *)PyUnicode_FromString("\\");
    }
    else if (length >= 3 && PyUnicode_READ(kind, data, 1) == ':' && PyUnicode_READ(kind, data, 2) == '\\')
    {
        return (PyUnicodeObject *)PyUnicode_FromString("\\");
    }
    else
    {
        return (PyUnicodeObject *)PyUnicode_FromString("");
    }
}

PyObject *
windows_root(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _windows_normalize(fspath);
    Py_DECREF(fspath);
    if (!normalized)
    {
        return NULL;
    }
    PyUnicodeObject *root = _windows_root(normalized);
    Py_DECREF(normalized);
    return (PyObject *)root;
}

// MARK: Name

PyUnicodeObject *
_windows_name(PyUnicodeObject *arg)
{
    Py_ssize_t arg_size = PyUnicode_GET_LENGTH(arg);
    unsigned int arg_kind = PyUnicode_KIND(arg);
    void *arg_data = PyUnicode_DATA(arg);

    Py_ssize_t unc_index = _windows_unc_index_impl(arg_size, arg_kind, arg_data);
    Py_ssize_t i = arg_size - 1;

    // Read until the next slash or the start of the string.
    while (i >= unc_index && PyUnicode_READ(arg_kind, arg_data, i) != '\\')
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
        return (PyUnicodeObject *)PyUnicode_Substring((PyObject *)arg, start, arg_size);
    }
}

PyObject *
windows_name(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _windows_normalize(fspath);
    Py_DECREF(fspath);
    if (!normalized)
    {
        return NULL;
    }
    PyUnicodeObject *name = _windows_name(normalized);
    Py_DECREF(normalized);
    return (PyObject *)name;
}

// MARK: Parent

Py_ssize_t
_windows_parent_index(PyUnicodeObject *arg)
{
    Py_ssize_t arg_size = PyUnicode_GET_LENGTH(arg);
    unsigned int arg_kind = PyUnicode_KIND(arg);
    void *arg_data = PyUnicode_DATA(arg);

    Py_ssize_t unc_index = _windows_unc_index_impl(arg_size, arg_kind, arg_data);
    Py_ssize_t i = arg_size;
    while (i > unc_index && PyUnicode_READ(arg_kind, arg_data, i - 1) != '\\')
    {
        i -= 1;
    }

    return i == unc_index ? 0 : i;
}

PyObject *
windows_parent(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _windows_normalize(fspath);
    Py_DECREF(fspath);
    if (!normalized)
    {
        return NULL;
    }
    Py_ssize_t parent_index = _windows_parent_index(normalized);
    Py_DECREF(normalized);
    if (parent_index > 0)
    {
        return PyUnicode_Substring(arg, 0, parent_index);
    }
    else
    {
        Py_RETURN_NONE;
    }
}
