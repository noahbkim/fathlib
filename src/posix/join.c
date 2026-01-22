#include "posix/join.h"

#include "common.h"
#include "posix/normalize.h"
#include "posix/view.h"

// MARK: Join

PyUnicodeObject *
_posix_join_zero()
{
    return (PyUnicodeObject *)PyUnicode_FromString(".");
}

PyUnicodeObject *
_posix_join_one(PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _posix_normalize(fspath);
    Py_DECREF(fspath);
    return normalized;
}

PyUnicodeObject *
_posix_join_many(PyObject **args, Py_ssize_t nargs)
{
    PyUnicodeObject *joined = NULL;
    PyUnicodeObject **fspaths = calloc(nargs, sizeof(PyUnicodeObject *));
    Py_ssize_t i = nargs;
    do
    {
        i -= 1;
        PyUnicodeObject *fspath = _fspath((PyObject *)args[i]);
        if (!fspath)
        {
            goto error;
        }
        fspaths[i] = fspath;
        if (_posix_is_absolute(fspath))
        {
            break;
        }
    }
    while (i > 0);

    // Last element is absolute, do no work
    if (i == nargs - 1)
    {
        joined = _posix_normalize(fspaths[0]);
        goto error; // think of this as goto done
    }

    Py_ssize_t write_size = PyUnicode_GET_LENGTH(fspaths[i]);
    unsigned int write_maxchar = PyUnicode_MAX_CHAR_VALUE(fspaths[i]);
    for (Py_ssize_t j = i + 1; j < nargs; ++j)
    {
        write_size += 1;
        write_size += PyUnicode_GET_LENGTH(fspaths[j]);
        unsigned int arg_maxchar = PyUnicode_MAX_CHAR_VALUE(fspaths[j]);
        write_maxchar = Py_MAX(write_maxchar, arg_maxchar);
    }

    PyUnicodeObject *write = (PyUnicodeObject *)PyUnicode_New(write_size, write_maxchar);
    if (!write)
    {
        return NULL;
    }

    Cow cow;
    cow_construct_writer(&cow, write);
    PosixNormalize state;
    if (_posix_normalize_impl(&cow, &state, fspaths[i]) != 0)
    {
        goto error;
    }
    for (Py_ssize_t j = i + 1; j < nargs; ++j)
    {
        cow_write(&cow, '/');
        if (_posix_normalize_impl(&cow, &state, fspaths[j]) != 0)
        {
            goto error;
        }
    }

    if (cow.write_index == 0)
    {
        cow_write(&cow, '.');
    }

    joined = cow_consume(&cow);

error:
    for (Py_ssize_t j = i; j < nargs; ++j)
    {
        Py_XDECREF(fspaths[j]);
    }
    free(fspaths);
    return joined;
}

PyUnicodeObject *
_posix_join(PyObject **args, Py_ssize_t nargs)
{
    switch (nargs)
    {
    case 0:
        return _posix_join_zero();
    case 1:
        return _posix_join_one(args[0]);
    default:
        return _posix_join_many(args, nargs);
    }
}

PyObject *
posix_join(PyObject *module, PyObject **args, Py_ssize_t nargs)
{
    if (nargs == 0)
    {
        return PyUnicode_FromStringAndSize(".", 1);
    }
    else
    {
        PyUnicodeObject *joined = _posix_join(args, nargs);
        if (!joined)
        {
            return NULL;
        }
        PyUnicodeObject *normalized = _posix_normalize(joined);
        Py_DECREF(joined);
        return (PyObject *)normalized;
    }
}

// MARK: Concat

PyUnicodeObject *
_posix_concat(PyObject **args, Py_ssize_t nargs)
{
    if (nargs == 0)
    {
        return (PyUnicodeObject *)PyUnicode_FromString(".");
    }
    else if (nargs == 1)
    {
        PyUnicodeObject *fspath = _fspath(args[0]);
        if (!fspath)
        {
            return NULL;
        }
        PyUnicodeObject *normalized = _posix_normalize(fspath);
        Py_DECREF(fspath);
        return normalized;
    }
    else
    {
        PyUnicodeObject *joined = NULL;
        PyUnicodeObject **fspaths = malloc(sizeof(PyUnicodeObject *) * nargs);

        for (Py_ssize_t i = 0; i < nargs; ++i)
        {
            PyUnicodeObject *fspath = _fspath((PyObject *)args[i]);
            if (!fspath)
            {
                goto error;
            }
            fspaths[i] = fspath;
        }

        joined = _join('/', fspaths, nargs);

error:
        free(fspaths);
        return joined;
    }
}

PyObject *
posix_concat(PyObject *module, PyObject **args, Py_ssize_t nargs)
{
    if (nargs == 0)
    {
        return PyUnicode_FromStringAndSize(".", 1);
    }
    else
    {
        PyUnicodeObject *joined = _posix_concat(args, nargs);
        if (!joined)
        {
            return NULL;
        }
        PyUnicodeObject *normalized = _posix_normalize(joined);
        Py_DECREF(joined);
        return (PyObject *)normalized;
    }
}
