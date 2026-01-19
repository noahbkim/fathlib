#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "posix/fath.h"
#include "posix/join.h"
#include "posix/normalize.h"
#include "posix/view.h"
#include "windows/fath.h"
#include "windows/join.h"
#include "windows/normalize.h"
#include "windows/view.h"

static PyMethodDef PyFathlibModule_methods[] = {
    {"posix_normalize_slash", (PyCFunction)posix_normalize_slash, METH_O,       PyDoc_STR("Normalize slashes")                       },
    {"posix_normalize_dot",   (PyCFunction)posix_normalize_dot,   METH_O,       PyDoc_STR("Normalize dot components")                },
    {"posix_normalize",       (PyCFunction)posix_normalize,       METH_O,       PyDoc_STR("Normalize a POSIX path")                  },
    {"posix_join",            (PyCFunction)posix_join,            METH_VARARGS, PyDoc_STR("Join parts into a POSIX path")            },
    {"posix_root",            (PyCFunction)posix_root,            METH_O,       PyDoc_STR("Get the root part of a POSIX path")       },
    {"posix_name",            (PyCFunction)posix_name,            METH_O,       PyDoc_STR("Get the name part of a POSIX path")       },
    {"posix_parent",          (PyCFunction)posix_parent,          METH_O,       PyDoc_STR("Get the parent of a POSIX path")          },
    {"windows_normalize",     (PyCFunction)windows_normalize,     METH_O,       PyDoc_STR("Normalize a Windows path")                },
    {"windows_as_posix",      (PyCFunction)windows_as_posix,      METH_O,       PyDoc_STR("Render a Windows path with POSIX slashes")},
    {"windows_join",          (PyCFunction)windows_join,          METH_VARARGS, PyDoc_STR("Join parts into a Windows path")          },
    {"windows_drive",         (PyCFunction)windows_drive,         METH_O,       PyDoc_STR("Get the drive part of a Windows path")    },
    {"windows_root",          (PyCFunction)windows_root,          METH_O,       PyDoc_STR("Get the root part of a Windows path")     },
    {"windows_name",          (PyCFunction)windows_name,          METH_O,       PyDoc_STR("Get the name part of a Windows path")     },
    {"windows_parent",        (PyCFunction)windows_parent,        METH_O,       PyDoc_STR("Get the parent of a Windows path")        },
    {NULL,                    NULL,                               0,            NULL                                                 }
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
