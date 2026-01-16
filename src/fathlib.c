#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "join.h"
#include "normalize.h"
#include "posix.h"
#include "windows.h"

static PyMethodDef PyFathlibModule_methods[] = {
    {"normalize_slash", (PyCFunction)normalize_slash, METH_O,       PyDoc_STR("Normalize forward slashes in a POSIX path")},
    {"normalize_dot",   (PyCFunction)normalize_dot,   METH_O,       PyDoc_STR("Normalize dot components in a POSIX path") },
    {"normalize_posix", (PyCFunction)normalize_posix, METH_O,       PyDoc_STR("Normalize a POSIX path")                   },
    {"join_posix",      (PyCFunction)join_posix,      METH_VARARGS, PyDoc_STR("Normalize a POSIX path")                   },
    {NULL,              NULL,                         0,            NULL                                                  }
};

static struct PyModuleDef PyFathlibModule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "core",
    .m_doc = PyDoc_STR("Bindings for core Fathlib types."),
    .m_size = -1, // Size of per-interpreter state
    .m_methods = PyFathlibModule_methods,
};

PyMODINIT_FUNC
PyInit_core()
{
    PyObject *module = NULL;

    if (PyType_Ready(&PyPosixFath_Type) < 0)
    {
        goto error;
    }

    if (PyType_Ready(&PyWindowsFath_Type) < 0)
    {
        goto error;
    }

    module = PyModule_Create(&PyFathlibModule);
    if (module == NULL)
    {
        goto error;
    }

    if (PyModule_AddObjectRef(module, "PosixFath", (PyObject *)&PyPosixFath_Type) != 0)
    {
        goto error;
    }

    if (PyModule_AddObjectRef(module, "WindowsFath", (PyObject *)&PyWindowsFath_Type) != 0)
    {
        goto error;
    }

    return module;

error:
    Py_XDECREF(module);
    return NULL;
}
