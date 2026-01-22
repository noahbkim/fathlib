#include "posix/join.h"

#include "common.h"
#include "posix/normalize.h"
#include "posix/view.h"

// MARK: Join

PyUnicodeObject *
_posix_join(PyObject **args, Py_ssize_t nargs)
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

        joined = _join('/', fspaths + i, nargs - i);

error:
        // todo decref fspaths
        free(fspaths);
        return joined;
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
