#ifndef WINDOWS_UNC_H
#define WINDOWS_UNC_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

Py_ssize_t _windows_unc_index_impl(Py_ssize_t arg_size, unsigned int arg_kind, void *arg_data);
Py_ssize_t _windows_unc_index(PyUnicodeObject *arg);
PyObject *windows_is_unc(PyObject *module, PyObject *arg);
PyObject *windows_unc(PyObject *module, PyObject *arg);

Py_ssize_t _windows_volume_index_impl(Py_ssize_t arg_size, unsigned int arg_kind, void *arg_data);
Py_ssize_t _windows_volume_index(PyUnicodeObject *arg);
PyObject *windows_is_volume(PyObject *module, PyObject *arg);
PyObject *windows_volume(PyObject *module, PyObject *arg);

Py_ssize_t _windows_device_index_impl(Py_ssize_t arg_size, unsigned int arg_kind, void *arg_data);
Py_ssize_t _windows_device_index(PyUnicodeObject *arg);
PyObject *windows_is_device(PyObject *module, PyObject *arg);
PyObject *windows_device(PyObject *module, PyObject *arg);

#endif
