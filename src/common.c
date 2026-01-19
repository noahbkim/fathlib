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
