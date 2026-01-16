#include "windows/normalize.h"
#include "common.h"

// MARK: Slashes

typedef enum
{
    NORMALIZE_SLASH_START,
    NORMALIZE_SLASH_START_SLASHES,
    NORMALIZE_SLASH_REST,
    NORMALIZE_SLASH_REST_SLASH,
    NORMALIZE_SLASH_REST_SLASH_SLASHES,
} NormalizeSlash;

PyUnicodeObject *
_windows_normalize_slash(PyUnicodeObject *read)
{
    unsigned int read_kind = PyUnicode_KIND(read);
    Py_ssize_t read_size = PyUnicode_GET_LENGTH(read);
    void *read_data = PyUnicode_DATA(read);

    PyUnicodeObject *write = NULL;
    unsigned int write_kind = read_kind; // just for readability
    void *write_data = NULL;

    Py_ssize_t read_index = 0;
    Py_ssize_t write_index = 0;
    NormalizeSlash state = NORMALIZE_SLASH_START;
    while (read_index < read_size)
    {
        Py_UCS4 character = PyUnicode_READ(read_kind, read_data, read_index);
        switch (state)
        {
        case NORMALIZE_SLASH_START:
            switch (character)
            {
            case '\\':
                write_index += 1;
                state = NORMALIZE_SLASH_START_SLASHES;
                break;
            case '/':
                if (_cow_copy(read, read_size, read_kind, read_data, &write, &write_data) != 0)
                {
                    return NULL;
                }
                PyUnicode_WRITE(write_kind, write_data, write_index, '\\');
                write_index += 1;
                state = NORMALIZE_SLASH_START_SLASHES;
                break;
            default:
                write_index += 1;
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_START_SLASHES:
            switch (character)
            {
            case '\\':
                if (write)
                {
                    PyUnicode_WRITE(write_kind, write_data, write_index, '\\');
                    write_index += 1;
                }
                else
                {
                    write_index += 1;
                }
                break;
            case '/':
                if (!write)
                {
                    if (_cow_copy(read, read_size, read_kind, read_data, &write, &write_data) != 0)
                    {
                        return NULL;
                    }
                }
                PyUnicode_WRITE(write_kind, write_data, write_index, '\\');
                write_index += 1;
                break;
            default:
                write_index += 1;
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_REST:
            switch (character)
            {
            case '\\':
                state = NORMALIZE_SLASH_REST_SLASH;
                break;
            case '/':
                // forces a `_cow_copy` on non-slash characters
                state = NORMALIZE_SLASH_REST_SLASH_SLASHES;
                break;
            default:
                if (write)
                {
                    PyUnicode_WRITE(write_kind, write_data, write_index, character);
                    write_index += 1;
                }
                else
                {
                    write_index += 1;
                }
            }
            break;
        case NORMALIZE_SLASH_REST_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                state = NORMALIZE_SLASH_REST_SLASH_SLASHES;
                break;
            default:
                if (write)
                {
                    PyUnicode_WRITE(write_kind, write_data, write_index, '\\');
                    write_index += 1;
                    PyUnicode_WRITE(write_kind, write_data, write_index, character);
                    write_index += 1;
                    state = NORMALIZE_SLASH_REST;
                }
                else
                {
                    write_index += 1;
                    write_index += 1;
                    state = NORMALIZE_SLASH_REST;
                }
            }
            break;
        case NORMALIZE_SLASH_REST_SLASH_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            default:
                if (!write)
                {
                    if (_cow_copy(read, read_size, read_kind, read_data, &write, &write_data) != 0)
                    {
                        return NULL;
                    }
                }
                PyUnicode_WRITE(write_kind, write_data, write_index, '\\');
                write_index += 1;
                PyUnicode_WRITE(write_kind, write_data, write_index, character);
                write_index += 1;
                state = NORMALIZE_SLASH_REST;
            }
            break;
        }
        read_index += 1;
    }
    return _cow_consume(read, read_size, read_kind, read_data, write, write_index);
}

PyObject *
windows_normalize_slash(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *inner = _fspath(arg);
    if (!inner)
    {
        return NULL;
    }
    return (PyObject *)_windows_normalize_slash(inner);
}

// MARK: Dot

typedef enum
{
    NORMALIZE_DOT_REST,
    NORMALIZE_DOT_REST_DOT,
    NORMALIZE_DOT_REST_DOT_SLASH,
} NormalizeDot;

PyUnicodeObject *
_windows_normalize_dot(PyUnicodeObject *read)
{
    unsigned int read_kind = PyUnicode_KIND(read);
    Py_ssize_t read_size = PyUnicode_GET_LENGTH(read);
    void *read_data = PyUnicode_DATA(read);

    PyUnicodeObject *write = NULL;
    unsigned int write_kind = read_kind; // just for readability
    void *write_data = NULL;

    Py_ssize_t read_index = 0;
    Py_ssize_t write_index = 0;
    NormalizeDot state = NORMALIZE_DOT_REST;
    while (read_index < read_size)
    {
        Py_UCS4 character = PyUnicode_READ(read_kind, read_data, read_index);
        switch (state)
        {
        case NORMALIZE_DOT_REST:
            if (character == '.')
            {
                state = NORMALIZE_DOT_REST_DOT;
            }
            else
            {
                state = NORMALIZE_DOT_REST;
                if (write)
                {
                    PyUnicode_WRITE(write_kind, write_data, write_index, character);
                    write_index += 1;
                }
                else
                {
                    write_index += 1;
                }
            }
            break;
        case NORMALIZE_DOT_REST_DOT:
            switch (character)
            {
            case '\\':
            case '/':
                state = NORMALIZE_DOT_REST_DOT_SLASH;
                break;
            default:
                if (write)
                {
                    PyUnicode_WRITE(write_kind, write_data, write_index, character);
                    write_index += 1;
                }
                else
                {
                    write_index += 1;
                }
                state = NORMALIZE_DOT_REST;
            }
            break;
        case NORMALIZE_DOT_REST_DOT_SLASH:
            if (character == '.')
            {
                state = NORMALIZE_DOT_REST_DOT;
            }
            else
            {
                if (!write)
                {
                    int status = _cow_copy(read, read_size, read_kind, read_data, &write, &write_data);
                    if (status != 0)
                    {
                        return NULL;
                    }
                }
                state = NORMALIZE_DOT_REST;
                PyUnicode_WRITE(write_kind, write_data, write_index, character);
                write_index += 1;
            }
            break;
        }
        read_index += 1;
    }

    // This can probably be fit into the FSA, but we need to ensure at least
    // one leading dot is included. This case gets hit for e.g. "./.".
    if (write_index == 0 && read_index > 0)
    {
        write_index = 1;
    }

    return _cow_consume(read, read_size, read_kind, read_data, write, write_index);
}

PyObject *
windows_normalize_dot(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *inner = _fspath(arg);
    if (!inner)
    {
        return NULL;
    }
    return (PyObject *)_windows_normalize_dot(inner);
}

// MARK: Windows

PyUnicodeObject *
_windows_normalize(PyUnicodeObject *read)
{
    Py_ssize_t length = PyUnicode_GET_LENGTH(read);

    // Replace an empty string with a ".".
    if (length == 0)
    {
        Py_DECREF(read);
        return (PyUnicodeObject *)PyUnicode_FromStringAndSize(".", 1);
    }

    // There is no invalid, one-character path.
    if (length == 1)
    {
        return read;
    }

    read = _windows_normalize_slash(read);
    if (!read)
    {
        return NULL;
    }

    read = _windows_normalize_dot(read);
    if (!read)
    {
        return NULL;
    }

    return read;
}

PyObject *
windows_normalize(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *inner = _fspath(arg);
    if (!inner)
    {
        return NULL;
    }
    return (PyObject *)_windows_normalize(inner);
}
