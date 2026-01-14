#include "cow.h"

void cow_construct(Cow *self, PyUnicodeObject *referenced)
{
    *self = {referenced, PyUnicode_DATA(referenced), NULL, NULL, 0, PyUnicode_KIND(referenced)};
}

int cow_push(Cow *self, Py_UCS4 character)
{
    if (self->owned == NULL)
    {
        Py_UCS4 referenced = PyUnicode_READ(self->kind, self->referenced_data, self->cursor);
        if (referenced == character)
        {
            self->cursor += 1;
            return 0;
        }
        else
        {
            Py_ssize_t size = PyUnicode_GET_LENGTH(self->referenced);
            self->owned = (PyUnicodeObject *)PyUnicode_FromKindAndData(self->kind, self->referenced_data, size);
            if (!self->owned)
            {
                return -1;
            }
            self->owned_data = PyUnicode_DATA(self->owned);
            return 0;
        }
    }
    else
    {
        PyUnicode_WRITE(self->kind, self->owned_data, self->cursor, character);
        self->cursor += 1;
        return 0;
    }
}

PyUnicodeObject *cow_consume(Cow *self)
{
    if (self->owned)
    {
        if (self->owned != self->referenced)
        {
            Py_DECREF(self->referenced);
        }
        if (PyUnicode_Resize((PyObject **)&self->owned, self->cursor) != 0)
        {
            Py_DECREF(self->owned);
            return NULL;
        }
        return self->owned;
    }
    else if (self->cursor != PyUnicode_GET_LENGTH(self->referenced_data))
    {
        PyObject *truncated = PyUnicode_FromKindAndData(self->kind, self->referenced_data, self->cursor);
        Py_DECREF(self->referenced);
        return (PyUnicodeObject *)truncated;
    }
    else
    {
        return self->referenced;
    }
}