#include "windows/normalize.h"
#include "common.h"
#include "cow.h"

#define COW_WRITE(SELF, READ)                                                                                          \
    if (cow_write((SELF), (READ)) != 0)                                                                                \
    {                                                                                                                  \
        return NULL;                                                                                                   \
    }

// MARK: Slashes

typedef enum
{
    NORMALIZE_SLASH_START,
    NORMALIZE_SLASH_START_SLASH,
    NORMALIZE_SLASH_START_SLASH_SLASH,
    NORMALIZE_SLASH_START_SLASH_SLASH_SLASHES,
    NORMALIZE_SLASH_UNC,
    NORMALIZE_SLASH_UNC_SLASH,
    NORMALIZE_SLASH_UNC_SLASH_SLASHES,
    NORMALIZE_SLASH_REST,
    NORMALIZE_SLASH_REST_SLASHES,
} NormalizeSlash;

PyUnicodeObject *
_windows_normalize_slash(PyUnicodeObject *read)
{
    Cow cow;
    cow_construct(&cow, read);

    NormalizeSlash state = NORMALIZE_SLASH_START;
    while (cow.read_index < cow.read_size)
    {
        Py_UCS4 character = PyUnicode_READ(cow.read_kind, cow.read_data, cow.read_index);
        switch (state)
        {
        case NORMALIZE_SLASH_START:
            switch (character)
            {
            case '\\':
            case '/':
                COW_WRITE(&cow, '\\');
                state = NORMALIZE_SLASH_START_SLASH;
                break;
            default:
                COW_WRITE(&cow, character);
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_START_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_WRITE(&cow, '\\');
                state = NORMALIZE_SLASH_START_SLASH_SLASH;
                break;
            default:
                COW_WRITE(&cow, character);
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_START_SLASH_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_WRITE(&cow, '\\');
                state = NORMALIZE_SLASH_START_SLASH_SLASH_SLASHES;
                break;
            default:
                COW_WRITE(&cow, character);
                state = NORMALIZE_SLASH_UNC;
            }
            break;
        case NORMALIZE_SLASH_START_SLASH_SLASH_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                COW_WRITE(&cow, '\\');
                break;
            default:
                COW_WRITE(&cow, character);
                state = NORMALIZE_SLASH_REST;
                break;
            }
            break;
        case NORMALIZE_SLASH_UNC:
            switch (character)
            {
            case '\\':
            case '/':
                COW_WRITE(&cow, '\\');
                state = NORMALIZE_SLASH_UNC_SLASH;
                break;
            default:
                COW_WRITE(&cow, character);
            }
            break;
        case NORMALIZE_SLASH_UNC_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_WRITE(&cow, '\\');
                state = NORMALIZE_SLASH_UNC_SLASH_SLASHES;
                break;
            default:
                COW_WRITE(&cow, character);
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_UNC_SLASH_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            default:
                COW_WRITE(&cow, character);
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_REST:
            switch (character)
            {
            case '\\':
            case '/':
                state = NORMALIZE_SLASH_REST_SLASHES;
                break;
            default:
                COW_WRITE(&cow, character);
            }
            break;
        case NORMALIZE_SLASH_REST_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            default:
                COW_WRITE(&cow, '\\');
                COW_WRITE(&cow, character);
            }
            break;
        }
        cow.read_index += 1;
    }
    return cow_consume(&cow);
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
    Py_ssize_t read_size = PyUnicode_GET_LENGTH(read);
    unsigned int read_kind = PyUnicode_KIND(read);
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
                if (!write && _cow_copy(read, read_size, read_kind, read_data, &write, write_index, &write_data) != 0)
                {
                    return NULL;
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
        return (PyUnicodeObject *)PyUnicode_FromStringAndSize(".", 1);
    }

    // Let this case handle "/".
    read = _windows_normalize_slash(read);
    if (!read)
    {
        return NULL;
    }

    // There can't be dot parts with length 1.
    if (length == 1)
    {
        return read;
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
