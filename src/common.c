#include "common.h"

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

int
_cow_copy(PyUnicodeObject *read,
          Py_ssize_t read_size,
          unsigned int read_kind,
          void *read_data,
          PyUnicodeObject **write,
          Py_ssize_t write_index,
          void **write_data)
{
    if (*write)
    {
        return 0;
    }
    else
    {
        *write = (PyUnicodeObject *)PyUnicode_New(read_size, PyUnicode_MAX_CHAR_VALUE(read));
        if (!*write)
        {
            return -1;
        }
        *write_data = PyUnicode_DATA(*write);
        for (Py_ssize_t i = 0; i < write_index; ++i)
        {
            PyUnicode_WRITE(read_kind, *write_data, i, PyUnicode_READ(read_kind, read_data, i));
        }
        return 0;
    }
}

// MARK: Cow

PyUnicodeObject *
_cow_consume(PyUnicodeObject *read,
             Py_ssize_t read_size,
             unsigned int read_kind,
             void *read_data,
             PyUnicodeObject *write,
             Py_ssize_t write_size)
{
    if (write)
    {
        if (write_size != read_size && PyUnicode_Resize((PyObject **)&write, write_size) != 0)
        {
            Py_DECREF(write);
            return NULL;
        }
        return write;
    }
    else if (write_size != read_size)
    {
        PyObject *truncated = PyUnicode_Substring((PyObject *)read, 0, write_size);
        return (PyUnicodeObject *)truncated;
    }
    else
    {
        Py_INCREF(read);
        return read;
    }
}
