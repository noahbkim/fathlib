#include "windows/normalize.h"

#include "common.h"
#include "cow.h"
#include "unicodeobject.h"
#include "windows/drive.h"

#define COW_ADVANCE(SELF, READ)                                                                                        \
    if (cow_advance((SELF), (READ)) != 0)                                                                              \
    {                                                                                                                  \
        goto error;                                                                                                    \
    }

int
_windows_normalize_impl(Cow *cow,
                        WindowsNormalizeState *state,
                        Py_ssize_t read_size,
                        unsigned int read_kind,
                        void *read_data,
                        Py_ssize_t read_index)
{
    for (; read_index < read_size; ++read_index)
    {
        Py_UCS4 character = PyUnicode_READ(cow->read_kind, cow->read_data, read_index);
        switch (*state)
        {
        case WINDOWS_NORMALIZE_STATE_START:
            switch (character)
            {
            case '\\':
            case '/':
                COW_ADVANCE(cow, '\\');
                *state = WINDOWS_NORMALIZE_STATE_START_SLASHES;
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_STATE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_STATE_START_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_STATE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_STATE_PART:
            switch (character)
            {
            case '\\':
            case '/':
                *state = WINDOWS_NORMALIZE_STATE_PART_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, character);
            }
            break;
        case WINDOWS_NORMALIZE_STATE_PART_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, '\\');
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_STATE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_STATE_DOT:
            switch (character)
            {
            case '\\':
            case '/':
                *state = WINDOWS_NORMALIZE_STATE_DOT_SLASHES;
                break;
            default:
                COW_ADVANCE(cow, '.');
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_STATE_PART;
            }
            break;
        case WINDOWS_NORMALIZE_STATE_DOT_SLASHES:
            switch (character)
            {
            case '\\':
            case '/':
                break;
            case '.':
                *state = WINDOWS_NORMALIZE_STATE_DOT;
                break;
            default:
                COW_ADVANCE(cow, character);
                *state = WINDOWS_NORMALIZE_STATE_PART;
            }
            break;
        }
    }
    return 0;

error:
    return -1;
}

PyUnicodeObject *
_windows_normalize(PyUnicodeObject *arg)
{
    Py_ssize_t arg_size = PyUnicode_GET_LENGTH(arg);
    if (arg_size == 0)
    {
        return (PyUnicodeObject *)PyUnicode_FromStringAndSize(".", 1);
    }

    Cow cow;
    cow_construct(&cow, arg);

    unsigned int arg_kind = PyUnicode_KIND(arg);
    void *arg_data = PyUnicode_DATA(arg);
    WindowsDriveInfo drive = _windows_drive_info_impl(arg_size, arg_kind, arg_data);

    // If the whole thing is a drive, we need a slash at the end (darn).
    int drive_slash = _windows_drive_slash(&drive);
    if (drive_slash)
    {
        if (cow_copy(&cow, arg_size + 1) != 0)
        {
            goto error;
        }
    }

    // Keep the drive exactly, replacing forward slashes with back slashes.
    Py_ssize_t arg_index = 0;
    for (; arg_index < drive.index; ++arg_index)
    {
        Py_UCS4 character = PyUnicode_READ(arg_kind, arg_data, arg_index);
        switch (character)
        {
        case '\\':
        case '/':
            COW_ADVANCE(&cow, '\\');
            break;
        default:
            COW_ADVANCE(&cow, character);
        }
    }

    if (drive.index == arg_size)
    {
        if (drive_slash)
        {
            cow_write(&cow, '\\');
        }
        return cow_consume(&cow);
    }

    WindowsNormalizeState state = WINDOWS_NORMALIZE_STATE_START;
    if (_windows_normalize_impl(&cow, &state, arg_size, arg_kind, arg_data, arg_index) != 0)
    {
        goto error;
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
