#include "posix/join.h"

#include "common.h"
#include "posix/normalize.h"

// MARK: POSIX

PyUnicodeObject *
_posix_join(PyObject **args, Py_ssize_t nargs)
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
        return (PyObject *)_posix_normalize(joined);
    }
}
