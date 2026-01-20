#include "common.h"
#include "pymacro.h"
#include "unicodeobject.h"

// MARK: FSPath

PyUnicodeObject *
_fspath(PyObject *arg)
{
    if (PyUnicode_CheckExact(arg))
    {
        Py_INCREF(arg);
        return (PyUnicodeObject *)arg;
    }

    PyObject *fspath = PyOS_FSPath(arg);
    if (!fspath)
    {
        return NULL;
    }

    if (!PyUnicode_Check(fspath))
    {
        PyObject *cls = PyObject_Type(fspath);
        Py_DECREF(fspath);
        if (!cls)
        {
            return NULL;
        }

        PyObject *cls_name = PyType_GetName((PyTypeObject *)cls);
        Py_DECREF(cls);
        if (!cls_name)
        {
            return NULL;
        }

        PyErr_Format(PyExc_TypeError,
                     "argument should be a str or an os.PathLike object where __fspath__ returns a str, not %R",
                     cls_name);
        Py_DECREF(cls_name);

        return NULL;
    }

    return (PyUnicodeObject *)fspath;
}

// MARK: Join

PyUnicodeObject *
_join(Py_UCS4 separator, PyUnicodeObject *const *args, Py_ssize_t nargs)
{
    if (nargs == 0)
    {
        return (PyUnicodeObject *)PyUnicode_New(0, 0);
    }
    else if (nargs == 1)
    {
        Py_INCREF(args[0]);
        return args[0];
    }

    Py_ssize_t size = PyUnicode_GET_LENGTH(args[0]);
    unsigned int maxchar = PyUnicode_MAX_CHAR_VALUE(args[0]);
    for (Py_ssize_t i = 1; i < nargs; ++i)
    {
        size += 1;
        size += PyUnicode_GET_LENGTH(args[i]);
        unsigned int arg_maxchar = PyUnicode_MAX_CHAR_VALUE(args[i]);
        maxchar = Py_MAX(maxchar, arg_maxchar);
    }

    PyObject *joined = PyUnicode_New(size, maxchar);
    if (!joined)
    {
        return NULL;
    }

    Py_ssize_t cursor = 0;
    Py_ssize_t write = PyUnicode_CopyCharacters(joined, cursor, (PyObject *)args[0], 0, PyUnicode_GET_LENGTH(args[0]));
    if (write < 0)
    {
        goto error;
    }
    cursor += write;

    for (Py_ssize_t i = 1; i < nargs; ++i)
    {
        if (PyUnicode_WriteChar(joined, cursor, separator) != 0)
        {
            goto error;
        }
        cursor += 1;
        write = PyUnicode_CopyCharacters(joined, cursor, (PyObject *)args[i], 0, PyUnicode_GET_LENGTH(args[i]));
        if (write < 0)
        {
            goto error;
        }
        cursor += write;
    }

    return (PyUnicodeObject *)joined;

error:
    Py_DECREF(joined);
    return NULL;
}
