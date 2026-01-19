#include "windows/view.h"

#include "common.h"
#include "windows/normalize.h"

// MARK: UNC

// \\server\share
// \\?\UNC\server\share
// \\.\device
// \\?\device
typedef enum
{
    UNC_INDEX_START,
    UNC_INDEX_START_SLASH,
    UNC_INDEX_START_SLASH_SLASH,
    UNC_INDEX_START_SLASH_SLASH_SLASH,
    UNC_INDEX_START_SLASH_SLASH_QUESTION,
    UNC_INDEX_START_SLASH_SLASH_QUESTION_SLASH,
    UNC_INDEX_START_SLASH_SLASH_DOT,
    UNC_INDEX_U,
    UNC_INDEX_UN,
    UNC_INDEX_UNC,
    UNC_INDEX_UNC_SLASH,
    UNC_INDEX_SERVER,
    UNC_INDEX_SERVER_SLASH,
    UNC_INDEX_SERVER_SLASH_SLASH,
    UNC_INDEX_SHARE, // doubles as device
} UncIndex;

Py_ssize_t
_windows_unc_index(Py_ssize_t length, unsigned int kind, void *data)
{
    Py_ssize_t read_index = 0;
    Py_ssize_t unc_index = 0;
    UncIndex state = UNC_INDEX_START;
    while (read_index < length)
    {
        Py_UCS4 character = PyUnicode_READ(kind, data, read_index);
        switch (state)
        {
        case UNC_INDEX_START:
            if (character == '\\')
            {
                unc_index += 1;
                state = UNC_INDEX_START_SLASH;
            }
            else
            {
                return 0;
            }
            break;
        case UNC_INDEX_START_SLASH:
            if (character == '\\')
            {
                unc_index += 1;
                state = UNC_INDEX_START_SLASH_SLASH;
            }
            else
            {
                return 0;
            }
            break;
        case UNC_INDEX_START_SLASH_SLASH:
            switch (character)
            {
            case '\\':
                unc_index += 1;
                state = UNC_INDEX_START_SLASH_SLASH_SLASH;
                break;
            case '?':
                unc_index += 1;
                state = UNC_INDEX_START_SLASH_SLASH_QUESTION;
                break;
            case '.':
                unc_index += 1;
                state = UNC_INDEX_START_SLASH_SLASH_DOT;
                break;
            default:
                unc_index += 1;
                state = UNC_INDEX_SERVER;
                break;
            }
            break;
        case UNC_INDEX_START_SLASH_SLASH_SLASH:
            if (character == '\\')
            {
                return unc_index;
            }
            else
            {
                unc_index += 1;
                state = UNC_INDEX_SHARE;
            }
            break;
        case UNC_INDEX_START_SLASH_SLASH_QUESTION:
            if (character == '\\')
            {
                unc_index += 1;
                state = UNC_INDEX_START_SLASH_SLASH_QUESTION_SLASH;
            }
            else
            {
                unc_index += 1;
                state = UNC_INDEX_SHARE;
            }
            break;
        case UNC_INDEX_START_SLASH_SLASH_QUESTION_SLASH:
            switch (character)
            {
            case '\\':
                return unc_index;
            case 'u':
            case 'U':
                unc_index += 1;
                state = UNC_INDEX_U;
                break;
            default:
                unc_index += 1;
                state = UNC_INDEX_SHARE;
                break;
            }
            break;
        case UNC_INDEX_START_SLASH_SLASH_DOT:
            unc_index += 1;
            state = UNC_INDEX_SHARE;
            break;
        case UNC_INDEX_U:
            switch (character)
            {
            case 'n':
            case 'N':
                unc_index += 1;
                state = UNC_INDEX_UN;
                break;
            case '\\':
                return unc_index;
            default:
                unc_index += 1;
                state = UNC_INDEX_SHARE;
                break;
            }
            break;
        case UNC_INDEX_UN:
            switch (character)
            {
            case 'c':
            case 'C':
                unc_index += 1;
                state = UNC_INDEX_UNC;
                break;
            case '\\':
                return unc_index;
            default:
                unc_index += 1;
                state = UNC_INDEX_SHARE;
                break;
            }
            break;
        case UNC_INDEX_UNC:
            if (character == '\\')
            {
                unc_index += 1;
                state = UNC_INDEX_UNC_SLASH;
            }
            else
            {
                unc_index += 1;
                state = UNC_INDEX_SHARE;
            }
            break;
        case UNC_INDEX_UNC_SLASH:
            if (character == '\\')
            {
                return unc_index;
            }
            else
            {
                unc_index += 1;
                state = UNC_INDEX_SERVER;
            }
            break;
        case UNC_INDEX_SERVER:
            if (character == '\\')
            {
                unc_index += 1;
                state = UNC_INDEX_SERVER_SLASH;
            }
            else
            {
                unc_index += 1;
            }
            break;
        case UNC_INDEX_SERVER_SLASH:
            if (character == '\\')
            {
                unc_index += 1;
                state = UNC_INDEX_SERVER_SLASH_SLASH;
            }
            else
            {
                unc_index += 1;
                state = UNC_INDEX_SHARE;
            }
            break;
        case UNC_INDEX_SERVER_SLASH_SLASH:
            if (character == '\\')
            {
                return unc_index;
            }
            else
            {
                unc_index += 1;
                state = UNC_INDEX_SHARE;
            }
            break;
        case UNC_INDEX_SHARE:
            if (character == '\\')
            {
                return unc_index;
            }
            else
            {
                unc_index += 1;
            }
            break;
        }
        read_index += 1;
    }
    return unc_index;
}

// MARK: Drive

PyUnicodeObject *
_windows_drive(PyUnicodeObject *read)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(read);
    int kind = PyUnicode_KIND(read);
    void *data = PyUnicode_DATA(read);

    if (length >= 1 && PyUnicode_READ(kind, data, 0) == '\\')
    {
        Py_ssize_t unc_index = _windows_unc_index(length, kind, data);
        return (PyUnicodeObject *)PyUnicode_FromKindAndData(kind, data, unc_index);
    }
    else if (length >= 2 && PyUnicode_READ(kind, data, 1) == ':')
    {
        return (PyUnicodeObject *)PyUnicode_FromKindAndData(kind, data, 2);
    }
    else
    {
        return (PyUnicodeObject *)PyUnicode_FromString("");
    }
}

PyObject *
windows_drive(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *inner = _fspath(arg);
    if (!inner)
    {
        return NULL;
    }
    inner = _windows_normalize(inner);
    if (!inner)
    {
        return NULL;
    }
    return (PyObject *)_windows_drive(inner);
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
    Py_ssize_t length = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);

    Py_ssize_t unc_index = _windows_unc_index(length, kind, data);
    Py_ssize_t i = length - 1;

    // Read until the next slash or the start of the string.
    while (i >= unc_index && PyUnicode_READ(kind, data, i) != '\\')
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
windows_name(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _windows_normalize(fspath);
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
    Py_ssize_t length = PyUnicode_GET_LENGTH(arg);
    int kind = PyUnicode_KIND(arg);
    void *data = PyUnicode_DATA(arg);

    Py_ssize_t unc_index = _windows_unc_index(length, kind, data);
    Py_ssize_t i = length - 1;
    while (i >= unc_index && PyUnicode_READ(kind, data, i) != '\\')
    {
        i -= 1;
    }

    return i;
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
    if (!normalized)
    {
        return NULL;
    }
    Py_ssize_t parent_index = _windows_parent_index(normalized);
    if (parent_index > 0)
    {
        return PyUnicode_Substring(arg, 0, parent_index);
    }
    else
    {
        Py_RETURN_NONE;
    }
}
