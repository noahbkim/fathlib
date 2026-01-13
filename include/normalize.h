#ifndef NORMALIZE_H
#define NORMALIZE_H

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyUnicodeObject *normalize_slash(PyUnicodeObject *inner);
PyUnicodeObject *normalize_dot(PyUnicodeObject *inner);

#endif
