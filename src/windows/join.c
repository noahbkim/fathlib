#include "windows/join.h"

#include "common.h"
#include "windows/normalize.h"

// MARK: Join

PyUnicodeObject *
_windows_join(PyObject **args, Py_ssize_t nargs)
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

    joined = _join('\\', fspaths, nargs);

error:
    free(fspaths);
    return joined;
}

PyObject *
windows_join(PyObject *module, PyObject **args, Py_ssize_t nargs)
{
    if (nargs == 0)
    {
        return PyUnicode_FromStringAndSize(".", 1);
    }
    else
    {
        PyUnicodeObject *joined = _windows_join(args, nargs);
        if (!joined)
        {
            return NULL;
        }
        PyUnicodeObject *normalized = _windows_normalize(joined);
        Py_DECREF(joined);
        return (PyObject *)normalized;
    }
}

// MARK: Concat

PyUnicodeObject *
_windows_concat(PyObject **args, Py_ssize_t nargs)
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

    joined = _join('\\', fspaths, nargs);

error:
    free(fspaths);
    return joined;
}

PyObject *
windows_concat(PyObject *module, PyObject **args, Py_ssize_t nargs)
{
    if (nargs == 0)
    {
        return PyUnicode_FromStringAndSize(".", 1);
    }
    else
    {
        PyUnicodeObject *joined = _windows_concat(args, nargs);
        if (!joined)
        {
            return NULL;
        }
        PyUnicodeObject *normalized = _windows_normalize(joined);
        Py_DECREF(joined);
        return (PyObject *)normalized;
    }
}
