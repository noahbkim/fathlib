#include "windows/normalize.h"

#include "common.h"
#include "cow.h"

#define COW_ADVANCE(SELF, READ)                                                                                        \
    if (cow_advance((SELF), (READ)) != 0)                                                                              \
    {                                                                                                                  \
        goto error;                                                                                                    \
    }

int
_windows_normalize_impl(Cow *cow, WindowsNormalize *state, PyUnicodeObject *read)
{
    for (Py_ssize_t read_index = 0; read_index < cow->read_size; ++read_index)
    {
        Py_UCS4 character = PyUnicode_READ(cow->read_kind, cow->read_data, read_index);
        switch (*state)
        {
        case WINDOWS_NORMALIZE_START:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(cow, '\\');
                *state = WINDOWS_NORMALIZE_START_SLASH;
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_START_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(cow, '\\');
                *state = WINDOWS_NORMALIZE_START_SLASH_SLASH;
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_START_SLASH_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(cow, '\\');
                *state = WINDOWS_NORMALIZE_START_SLASH_SLASH_SLASH;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_SERVER;
            }
            break;
        case WINDOWS_NORMALIZE_START_SLASH_SLASH_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(cow, '\\');
                *state = WINDOWS_NORMALIZE_START_SLASH_SLASH_SLASH_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_SHARE;
            }
            break;
        case WINDOWS_NORMALIZE_START_SLASH_SLASH_SLASH_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_SERVER:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(cow, '\\');
                *state = WINDOWS_NORMALIZE_SERVER_SLASH;
                break;
            default:
                COW_ADVANCE(cow, character);
            }
            break;
        case WINDOWS_NORMALIZE_SERVER_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(cow, '\\');
                *state = WINDOWS_NORMALIZE_SERVER_SLASH_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_SHARE;
            }
            break;
        case WINDOWS_NORMALIZE_SERVER_SLASH_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_SHARE:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(cow, '\\');
                *state = WINDOWS_NORMALIZE_SHARE_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_SHARE;
            }
            break;
        case WINDOWS_NORMALIZE_SHARE_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_PART:
            switch (character)
            {
            case '\\':
            case '/':
                *state = WINDOWS_NORMALIZE_PART_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, character);
            }
            break;
        case WINDOWS_NORMALIZE_PART_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(cow, '\\');
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_DOT:
            switch (character)
            {
            case '\\':
            case '/':
                *state = WINDOWS_NORMALIZE_DOT_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, '.');
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_DOT_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_PART;
            }
            break;
        }
    }
    return 0;

error:
    return -1;
}

PyUnicodeObject *
_windows_normalize(PyUnicodeObject *read)
{
    if (PyUnicode_GET_LENGTH(read) == 0)
    {
        return (PyUnicodeObject *)PyUnicode_FromStringAndSize(".", 1);
    }

    Cow cow;
    cow_construct(&cow, read);
    WindowsNormalize state = WINDOWS_NORMALIZE_START;

    if (_windows_normalize_impl(&cow, &state, read) != 0)
    {
        cow_destroy(&cow);
        return NULL;
    }

    // This can probably be fit into the FSA, but we need to ensure at least
    // one leading dot is included. This case gets hit for e.g. "./.".
    if (cow.write_index == 0 && cow.read_size > 0)
    {
        cow.write_index = 1;
    }

    return cow_consume(&cow);
}

PyObject *
windows_normalize(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _windows_normalize(fspath);
    Py_DECREF(fspath);
    return (PyObject *)normalized;
}
