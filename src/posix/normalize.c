#include "posix/normalize.h"

#include "common.h"
#include "cow.h"

#define COW_ADVANCE(SELF, READ)                                                                                        \
    if (cow_advance((SELF), (READ)) != 0)                                                                              \
    {                                                                                                                  \
        goto error;                                                                                                    \
    }

int
_posix_normalize_state_impl(Cow *cow, PosixNormalizeState *state, PyUnicodeObject *read)
{
    for (Py_ssize_t read_index = 0; read_index < cow->read_size; ++read_index)
    {
        Py_UCS4 character = PyUnicode_READ(cow->read_kind, cow->read_data, read_index);
        switch (*state)
        {
        case POSIX_NORMALIZE_STATE_START:
            switch (character)
            {
            case '/':
                cow->write_index += 1;
                *state = POSIX_NORMALIZE_STATE_START_SLASH;
                break;
            case '.':
                *state = POSIX_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = POSIX_NORMALIZE_STATE_PART;
            }
            break;
        case POSIX_NORMALIZE_STATE_START_SLASH:
            switch (character)
            {
            case '/':
                cow->write_index += 1;
                *state = POSIX_NORMALIZE_STATE_START_SLASH_SLASH;
                break;
            case '.':
                *state = POSIX_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = POSIX_NORMALIZE_STATE_PART;
            }
            break;
        case POSIX_NORMALIZE_STATE_START_SLASH_SLASH:
            switch (character)
            {
            case '/':
                cow->write_index -= 1;
                *state = POSIX_NORMALIZE_STATE_START_SLASH_SLASH_SLASHES;
                break;
            case '.':
                *state = POSIX_NORMALIZE_STATE_DOT;
                break;
            default:
                cow->write_index += 1;
                *state = POSIX_NORMALIZE_STATE_PART;
            }
            break;
        case POSIX_NORMALIZE_STATE_START_SLASH_SLASH_SLASHES:
            switch (character)
            {
            case '/':
                break;
            case '.':
                *state = POSIX_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = POSIX_NORMALIZE_STATE_PART;
            }
            break;
        case POSIX_NORMALIZE_STATE_PART:
            switch (character)
            {
            case '/':
                *state = POSIX_NORMALIZE_STATE_PART_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, character);
            }
            break;
        case POSIX_NORMALIZE_STATE_PART_SLASHES:
            switch (character)
            {
            case '/':
                break;
            case '.':
                *state = POSIX_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, '/');
                COW_ADVANCE(cow, character);
                *state = POSIX_NORMALIZE_STATE_PART;
            }
            break;
        case POSIX_NORMALIZE_STATE_DOT:
            switch (character)
            {
            case '/':
                *state = POSIX_NORMALIZE_STATE_DOT_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, '.');
                COW_ADVANCE(cow, character);
                *state = POSIX_NORMALIZE_STATE_PART;
            }
            break;
        case POSIX_NORMALIZE_STATE_DOT_SLASHES:
            switch (character)
            {
            case '/':
                break;
            case '.':
                *state = POSIX_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = POSIX_NORMALIZE_STATE_PART;
            }
            break;
        }
    }

    return 0;

error:
    return -1;
}

PyUnicodeObject *
_posix_normalize(PyUnicodeObject *read)
{
    switch (PyUnicode_GET_LENGTH(read))
    {
    case 0:
        return (PyUnicodeObject *)PyUnicode_FromString(".");
    case 1:
        Py_INCREF(read);
        return read;
    }

    Cow cow;
    cow_construct(&cow, read);
    PosixNormalizeState state = POSIX_NORMALIZE_STATE_START;

    if (_posix_normalize_state_impl(&cow, &state, read) != 0)
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
posix_normalize(PyObject *module, PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    PyUnicodeObject *normalized = _posix_normalize(fspath);
    Py_DECREF(fspath);
    return (PyObject *)normalized;
}
