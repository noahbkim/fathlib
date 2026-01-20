#include "windows/normalize.h"

#include "common.h"
#include "cow.h"

#define COW_ADVANCE(SELF, READ)                                                                                        \
    if (cow_advance((SELF), (READ)) != 0)                                                                              \
    {                                                                                                                  \
        goto error;                                                                                                    \
    }

typedef enum
{
    NORMALIZE_START,
    NORMALIZE_START_SLASH,
    NORMALIZE_START_SLASH_SLASH,
    NORMALIZE_START_SLASH_SLASH_SLASH,
    NORMALIZE_START_SLASH_SLASH_SLASH_SLASHES,
    NORMALIZE_SERVER,
    NORMALIZE_SERVER_SLASH,
    NORMALIZE_SERVER_SLASH_SLASHES,
    NORMALIZE_SHARE,
    NORMALIZE_SHARE_SLASHES,
    NORMALIZE_PART,
    NORMALIZE_PART_SLASHES,
    NORMALIZE_DOT,
    NORMALIZE_DOT_SLASHES,
} NormalizeSlash;

PyUnicodeObject *
_windows_normalize(PyUnicodeObject *read)
{
    if (PyUnicode_GET_LENGTH(read) == 0)
    {
        return (PyUnicodeObject *)PyUnicode_FromStringAndSize(".", 1);
    }

    Cow cow;
    cow_construct(&cow, read);

    NormalizeSlash state = NORMALIZE_START;
    for (Py_ssize_t read_index = 0; read_index < cow.read_size; ++read_index)
    {
        Py_UCS4 character = PyUnicode_READ(cow.read_kind, cow.read_data, read_index);
        switch (state)
        {
        case NORMALIZE_START:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(&cow, '\\');
                state = NORMALIZE_START_SLASH;
                break;
            case '.':
                state = NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_PART;
            }
            break;
        case NORMALIZE_START_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(&cow, '\\');
                state = NORMALIZE_START_SLASH_SLASH;
                break;
            case '.':
                state = NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_PART;
            }
            break;
        case NORMALIZE_START_SLASH_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(&cow, '\\');
                state = NORMALIZE_START_SLASH_SLASH_SLASH;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_SERVER;
            }
            break;
        case NORMALIZE_START_SLASH_SLASH_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(&cow, '\\');
                state = NORMALIZE_START_SLASH_SLASH_SLASH_SLASHES;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_SHARE;
            }
            break;
        case NORMALIZE_START_SLASH_SLASH_SLASH_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                state = NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_PART;
            }
            break;
        case NORMALIZE_SERVER:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(&cow, '\\');
                state = NORMALIZE_SERVER_SLASH;
                break;
            default:
                COW_ADVANCE(&cow, character);
            }
            break;
        case NORMALIZE_SERVER_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(&cow, '\\');
                state = NORMALIZE_SERVER_SLASH_SLASHES;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_SHARE;
            }
            break;
        case NORMALIZE_SERVER_SLASH_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                state = NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_PART;
            }
            break;
        case NORMALIZE_SHARE:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(&cow, '\\');
                state = NORMALIZE_SHARE_SLASHES;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_SHARE;
            }
            break;
        case NORMALIZE_SHARE_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                state = NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_PART;
            }
            break;
        case NORMALIZE_PART:
            switch (character)
            {
            case '\\':
            case '/':
                state = NORMALIZE_PART_SLASHES;
                break;
            default:
                COW_ADVANCE(&cow, character);
            }
            break;
        case NORMALIZE_PART_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                state = NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(&cow, '\\');
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_PART;
            }
            break;
        case NORMALIZE_DOT:
            switch (character)
            {
            case '\\':
            case '/':
                state = NORMALIZE_DOT_SLASHES;
                break;
            default:
                COW_ADVANCE(&cow, '.');
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_PART;
            }
            break;
        case NORMALIZE_DOT_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                state = NORMALIZE_DOT;
                break;
            default:
                COW_ADVANCE(&cow, character);
                state = NORMALIZE_PART;
            }
            break;
        }
    }

    // This can probably be fit into the FSA, but we need to ensure at least
    // one leading dot is included. This case gets hit for e.g. "./.".
    if (cow.write_index == 0 && cow.read_size > 0)
    {
        cow.write_index = 1;
    }

    return cow_consume(&cow);

error:
    cow_destroy(&cow);
    return NULL;
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
