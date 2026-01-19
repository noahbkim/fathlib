#include "cow.h"

void
cow_construct(Cow *self, PyUnicodeObject *read)
{
    self->read = read;
    self->read_size = PyUnicode_GET_LENGTH(read);
    self->read_kind = PyUnicode_KIND(read);
    self->read_data = PyUnicode_DATA(read);
    self->read_index = 0;
    self->write = NULL;
    self->write_size = 0;
    self->write_kind = 0;
    self->write_data = NULL;
    self->write_index = 0;
}

int
cow_write(Cow *self, Py_UCS4 character)
{
    if (self->write)
    {
        assert(self->write_data);
        assert(self->write_index < self->write_size);
        PyUnicode_WRITE(self->write_kind, self->write_data, self->write_index, character);
        self->write_index += 1;
        return 0;
    }
    else if (PyUnicode_READ(self->read_kind, self->read_data, self->read_index) != character)
    {
        self->write = (PyUnicodeObject *)PyUnicode_New(self->read_size, PyUnicode_MAX_CHAR_VALUE(self->read));
        if (!self->write)
        {
            return -1;
        }
        self->write_size = self->read_size;
        self->write_kind = PyUnicode_KIND(self->write);
        self->write_data = PyUnicode_DATA(self->write);
        for (Py_ssize_t i = 0; i < self->write_index; ++i)
        {
            PyUnicode_WRITE(self->write_kind, self->write_data, i, PyUnicode_READ(self->read_kind, self->read_data, i));
        }
        PyUnicode_WRITE(self->write_kind, self->write_data, self->write_index, character);
        self->write_index += 1;
        return 0;
    }
    else
    {
        self->write_index += 1;
        return 0;
    }
}

PyUnicodeObject *
cow_consume(Cow *self)
{
    if (self->write)
    {
        if (self->write_index == self->write_size)
        {
            return self->write;
        }
        else
        {
            if (PyUnicode_Resize((PyObject **)&self->write, self->write_index) != 0)
            {
                Py_DECREF(self->write);
                return NULL;
            }
            return self->write;
        }
    }
    else if (self->write_index != self->read_size)
    {
        PyObject *truncated = PyUnicode_Substring((PyObject *)self->read, 0, self->write_index);
        return (PyUnicodeObject *)truncated;
    }
    else
    {
        Py_INCREF(self->read);
        return self->read;
    }
}
