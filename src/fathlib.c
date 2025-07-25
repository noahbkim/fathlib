#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "generic.h"
#include "posix.h"

static PyMethodDef PyFathlibModule_Methods[] = {
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef PyFathlibModule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "core",
    .m_doc = PyDoc_STR("Bindings for core Fathlib types."),
    .m_size = -1, // Size of per-interpreter state
    .m_methods = PyFathlibModule_Methods,
};

PyMODINIT_FUNC
PyInit_core()
{
    PyObject *module = NULL;

    // Cancel initialization if types aren't constructed yet.
    if (PyType_Ready(&PyPosixFath_Type) < 0)
    {
        return NULL;
    }

    // Construct the containing module object.
    module = PyModule_Create(&PyFathlibModule);
    if (module == NULL)
    {
        goto error;
    }

    // Add a reference to our `Fath` types.
    if (PyModule_AddObjectRef(module, "PurePosixFath", (PyObject *)&PyPosixFath_Type) < 0)
    {
        goto error;
    }

    return module;

error:
    Py_XDECREF(module);
    return NULL;
}
