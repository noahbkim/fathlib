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
    unsigned int valid : 1;
    Py_ssize_t index;
} WindowsDriveInfo;

WindowsDriveInfo _windows_drive_info_impl(Py_ssize_t arg_size, unsigned int arg_kind, void *arg_data);
WindowsDriveInfo _windows_drive_info(PyUnicodeObject *arg);
WindowsDriveKind _windows_drive_kind(PyUnicodeObject *arg);
Py_ssize_t _windows_drive_index(PyUnicodeObject *arg);
int _windows_drive_slash(WindowsDriveInfo *drive);
PyObject *windows_drive(PyObject *module, PyObject *arg);

#endif
