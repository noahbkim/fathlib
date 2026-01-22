#ifndef WINDOWS_UNC_H
#define WINDOWS_UNC_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

typedef enum
{
    WINDOWS_DRIVE_NONE,
    WINDOWS_DRIVE_VOLUME,
    WINDOWS_DRIVE_UNC,
    WINDOWS_DRIVE_DEVICE_LITERAL,
    WINDOWS_DRIVE_DEVICE_NORMALIZED,
    WINDOWS_DRIVE_DEVICE_UNC,
} WindowsDriveKind;

typedef struct
{
    WindowsDriveKind kind;
    Py_ssize_t index;
} WindowsDriveKindAndIndex;

WindowsDriveKindAndIndex _windows_drive_kind_and_index_impl(Py_ssize_t arg_size, unsigned int arg_kind, void *arg_data);
WindowsDriveKindAndIndex _windows_drive_kind_and_index(PyUnicodeObject *arg);
WindowsDriveKind _windows_drive_kind(PyUnicodeObject *arg);
Py_ssize_t _windows_drive_index(PyUnicodeObject *arg);
PyObject *windows_drive(PyObject *module, PyObject *arg);

#endif
