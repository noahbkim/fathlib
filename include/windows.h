#ifndef WINDOWS_H
#define WINDOWS_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "generic.h"

// MARK: Object

typedef PyFathObject PyWindowsFathObject;

// MARK: Type

extern PyTypeObject PyWindowsFath_Type;

#define PyWindowsFath_CheckExact(ob) Py_IS_TYPE((ob), &PyWindowsFath_Type)
#define PyWindowsFath_Check(ob) PyWindowsFath_CheckExact(ob) || PyType_IsSubtype(Py_TYPE(ob), &PyWindowsFath_Type)

#endif
