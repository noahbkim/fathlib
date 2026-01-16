#ifndef POSIX_H
#define POSIX_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "generic.h"

// MARK: Object

typedef PyFathObject PyPosixFathObject;

// MARK: Type

extern PyTypeObject PyPosixFath_Type;

#define PyPosixFath_CheckExact(ob) (Py_IS_TYPE((ob), &PyPosixFath_Type))
#define PyPosixFath_Check(ob) (PyPosixFath_CheckExact(ob) || PyType_IsSubtype(Py_TYPE(ob), &PyPosixFath_Type))

#endif
