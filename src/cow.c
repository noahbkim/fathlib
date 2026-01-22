#include "cow.h"

void
cow_construct(Cow *self, PyUnicodeObject *read)
{
    self->read = read;
    self->read_size = PyUnicode_GET_LENGTH(read);
    self->read_kind = PyUnicode_KIND(read);
    self->read_data = PyUnicode_DATA(read);
    self->write = NULL;
    self->write_size = 0;
    self->write_kind = 0;
    self->write_data = NULL;
    self->write_index = 0;
}

void
cow_construct_writer(Cow *self, PyUnicodeObject *write)
{
    self->write = write;
    self->write_size = PyUnicode_GET_LENGTH(write);
    self->write_kind = PyUnicode_KIND(write);
    self->write_data = PyUnicode_DATA(write);
    self->write_index = 0;
}

int
cow_copy(Cow *self, Py_ssize_t resize)
{
    if (self->write)
    {
        return 0;
    }
    self->write = (PyUnicodeObject *)PyUnicode_New(resize, PyUnicode_MAX_CHAR_VALUE(self->read));
    if (!self->write)
    {
        return -1;
    }
    self->write_size = resize;
    self->write_kind = PyUnicode_KIND(self->write);
    self->write_data = PyUnicode_DATA(self->write);
    for (Py_ssize_t i = 0; i < self->write_index; ++i)
    {
        PyUnicode_WRITE(self->write_kind, self->write_data, i, PyUnicode_READ(self->read_kind, self->read_data, i));
    }
    return 0;
}

int
cow_advance(Cow *self, Py_UCS4 character)
{
    if (self->write)
    {
        PyUnicode_WRITE(self->write_kind, self->write_data, self->write_index, character);
        self->write_index += 1;
        return 0;
    }
    else if (PyUnicode_READ(self->read_kind, self->read_data, self->write_index) != character)
    {
        cow_copy(self, self->read_size);
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

int
cow_write(Cow *self, Py_UCS4 character)
{
    if (self->write)
    {
        PyUnicode_WRITE(self->write_kind, self->write_data, self->write_index, character);
        self->write_index += 1;
        return 0;
    }
    else
    {
        cow_copy(self, self->read_size);
        PyUnicode_WRITE(self->write_kind, self->write_data, self->write_index, character);
        self->write_index += 1;
        return 0;
    }
}

int
cow_resize(Cow *self, Py_ssize_t size)
{
    if (self->write)
    {
        if (PyUnicode_Resize((PyObject **)&self->write, size) != 0)
        {
            return -1;
        }
        self->write_size = size;
        self->write_data = PyUnicode_DATA(self->write);
        return 0;
    }
    else
    {
        return cow_copy(self, size);
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

void
cow_destroy(Cow *self)
{
    Py_XDECREF(self->write);
}
