#include "posix/normalize.h"
#include "common.h"

// MARK: Slashes

typedef enum
{
    NORMALIZE_SLASH_START,
    NORMALIZE_SLASH_START_SLASH,
    NORMALIZE_SLASH_START_SLASH_SLASH,
    NORMALIZE_SLASH_START_SLASH_SLASH_SLASHES,
    NORMALIZE_SLASH_REST,
    NORMALIZE_SLASH_REST_SLASH,
    NORMALIZE_SLASH_REST_SLASH_SLASHES,
} NormalizeSlash;

PyUnicodeObject *
_posix_normalize_slash(PyUnicodeObject *read)
{
    Py_ssize_t read_size = PyUnicode_GET_LENGTH(read);
    unsigned int read_kind = PyUnicode_KIND(read);
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
            // Always advance the first character, but possibly transition into
            // a different state for starting slashes or dots.
            if (character == '/')
            {
                write_index += 1;
                state = NORMALIZE_SLASH_START_SLASH;
            }
            else
            {
                write_index += 1;
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_START_SLASH:
            // Always advance the second character. Check if we need to handle
            // a double-slash root or collapse three or more slashes.
            if (character == '/')
            {
                write_index += 1;
                state = NORMALIZE_SLASH_START_SLASH_SLASH;
            }
            else
            {
                write_index += 1;
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_START_SLASH_SLASH:
            // If there are three or more slashes, collapse them by setting the
            // `cursor` position after the first. If the string ends, we will
            // return a string with a single slash. If it doesn't, we can
            // resume writing non-slash characters.
            if (character == '/')
            {
                write_index = 1;
                state = NORMALIZE_SLASH_START_SLASH_SLASH_SLASHES;
            }
            else
            {
                write_index += 1;
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_START_SLASH_SLASH_SLASHES:
            // When we reach the first non-slash after three or more slashes,
            // we have to strip the redundant ones. We can do this by setting
            // `cursor` after the first and writing the rest of the path.
            if (character != '/')
            {
                int status = _cow_copy(read, read_size, read_kind, read_data, &write, &write_data);
                if (status != 0)
                {
                    return NULL;
                }
                // use slash already at start of string, `write_index` is 1
                PyUnicode_WRITE(write_kind, write_data, write_index, character);
                write_index += 1;
                state = NORMALIZE_SLASH_REST;
            }
            break;
        case NORMALIZE_SLASH_REST:
            // Once past the root of the path, normalize redundant slashes.
            if (character == '/')
            {
                state = NORMALIZE_SLASH_REST_SLASH;
            }
            else
            {
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
            if (character == '/')
            {
                state = NORMALIZE_SLASH_REST_SLASH_SLASHES;
            }
            else
            {
                if (write)
                {
                    state = NORMALIZE_SLASH_REST;
                    PyUnicode_WRITE(write_kind, write_data, write_index, '/');
                    write_index += 1;
                    PyUnicode_WRITE(write_kind, write_data, write_index, character);
                    write_index += 1;
                }
                else
                {
                    state = NORMALIZE_SLASH_REST;
                    write_index += 1;
                    write_index += 1;
                }
            }
            break;
        case NORMALIZE_SLASH_REST_SLASH_SLASHES:
            if (character != '/')
            {
                if (!write)
                {
                    int status = _cow_copy(read, read_size, read_kind, read_data, &write, &write_data);
                    if (status != 0)
                    {
                        return NULL;
                    }
                }
                PyUnicode_WRITE(write_kind, write_data, write_index, '/');
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
posix_normalize_slash(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *inner = _fspath(arg);
    if (!inner)
    {
        return NULL;
    }
    return (PyObject *)_posix_normalize_slash(inner);
}

// MARK: Dot

typedef enum
{
    NORMALIZE_DOT_REST,
    NORMALIZE_DOT_REST_DOT,
    NORMALIZE_DOT_REST_DOT_SLASH,
} NormalizeDot;

PyUnicodeObject *
_posix_normalize_dot(PyUnicodeObject *read)
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
        case NORMALIZE_DOT_REST_DOT:
            if (character == '/')
            {
                state = NORMALIZE_DOT_REST_DOT_SLASH;
            }
            else
            {
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
                PyUnicode_WRITE(write_kind, write_data, write_index, character);
                write_index += 1;
                state = NORMALIZE_DOT_REST;
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
posix_normalize_dot(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *inner = _fspath(arg);
    if (!inner)
    {
        return NULL;
    }
    return (PyObject *)_posix_normalize_dot(inner);
}

// MARK: Posix

PyUnicodeObject *
_posix_normalize(PyUnicodeObject *read)
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

    read = _posix_normalize_slash(read);
    if (!read)
    {
        return NULL;
    }

    read = _posix_normalize_dot(read);
    if (!read)
    {
        return NULL;
    }

    return read;
}

PyObject *
posix_normalize(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *inner = _fspath(arg);
    if (!inner)
    {
        return NULL;
    }
    return (PyObject *)_posix_normalize(inner);
}
