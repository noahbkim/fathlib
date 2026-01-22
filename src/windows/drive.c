#include "windows/drive.h"

#include "common.h"
#include "unicodeobject.h"

// https://www.fileside.app/blog/2023-03-17_windows-file-paths/
typedef enum
{
    WINDOWS_DRIVE_STATE_START,
    WINDOWS_DRIVE_STATE_START_SLASH,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_U,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_UN,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_UNC,
    WINDOWS_DRIVE_STATE_START_SLASH_SLASH_DOT,
    WINDOWS_DRIVE_STATE_START_OTHER,
    WINDOWS_DRIVE_STATE_UNC_SERVER,
    WINDOWS_DRIVE_STATE_UNC_SHARE,
    WINDOWS_DRIVE_STATE_DEVICE_LITERAL,
    WINDOWS_DRIVE_STATE_DEVICE_NORMALIZED,
} WindowsDriveState;

WindowsDriveKindAndIndex
_windows_drive_kind_and_index_impl(Py_ssize_t arg_size, unsigned int arg_kind, void *arg_data)
{
    WindowsDriveKindAndIndex drive;
    drive.kind = WINDOWS_DRIVE_NONE;
    drive.index = 0;

    WindowsDriveState state;
    for (drive.index = 0; drive.index < arg_size; ++drive.index)
    {
        Py_UCS4 character = PyUnicode_READ(arg_kind, arg_data, drive.index);
        switch (state)
        {
        case WINDOWS_DRIVE_STATE_START:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_START_SLASH;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_START_OTHER;
                break;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH;
                break;
            default:
                goto done;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                drive.kind = WINDOWS_DRIVE_UNC;
                state = WINDOWS_DRIVE_STATE_UNC_SHARE;
                break;
            case '?':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION;
                break;
            case '.':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_DOT;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_UNC_SERVER;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION:
            switch (character)
            {
            case '\\':
            case '/':
                drive.kind = WINDOWS_DRIVE_DEVICE_LITERAL;
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_UNC_SERVER;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH:
            switch (character)
            {
            case '\\':
            case '/':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH;
                break;
            case 'u':
            case 'U':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_U;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_UNC_SERVER;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_U:
            switch (character)
            {
            case '\\':
            case '/':
                goto done;
                break;
            case 'n':
            case 'N':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_UN;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_DEVICE_LITERAL;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_UN:
            switch (character)
            {
            case '\\':
            case '/':
                goto done;
                break;
            case 'c':
            case 'C':
                state = WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_UNC;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_DEVICE_LITERAL;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_QUESTION_SLASH_UNC:
            switch (character)
            {
            case '\\':
            case '/':
                drive.kind = WINDOWS_DRIVE_DEVICE_UNC;
                state = WINDOWS_DRIVE_STATE_UNC_SERVER;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_DEVICE_LITERAL;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_SLASH_SLASH_DOT:
            switch (character)
            {
            case '\\':
            case '/':
                drive.kind = WINDOWS_DRIVE_DEVICE_NORMALIZED;
                state = WINDOWS_DRIVE_STATE_DEVICE_NORMALIZED;
                break;
            default:
                state = WINDOWS_DRIVE_STATE_UNC_SERVER;
            }
            break;
        case WINDOWS_DRIVE_STATE_START_OTHER:
            switch (character)
            {
            case ':':
                drive.kind = WINDOWS_DRIVE_VOLUME;
                goto done;
                break;
            default:
                drive.index = 0;
                return drive;
            }
            break;
        case WINDOWS_DRIVE_STATE_UNC_SERVER:
            switch (character)
            {
            case '\\':
            case '/':
                drive.kind = WINDOWS_DRIVE_UNC;
                state = WINDOWS_DRIVE_STATE_UNC_SHARE;
                break;
            }
        case WINDOWS_DRIVE_STATE_UNC_SHARE:
            switch (character)
            {
            case '\\':
            case '/':
                goto done;
                break;
            }
            break;
        case WINDOWS_DRIVE_STATE_DEVICE_LITERAL:
            switch (character)
            {
            case '\\':
            case '/':
                goto done;
                break;
            }
            break;
        case WINDOWS_DRIVE_STATE_DEVICE_NORMALIZED:
            switch (character)
            {
            case '\\':
            case '/':
                goto done;
                break;
            }
            break;
        }
    }

done:
    if (drive.kind != WINDOWS_DRIVE_NONE)
    {
        drive.index += 1;
        return drive;
    }
    else
    {
        drive.index = 0;
        return drive;
    }
}

WindowsDriveKindAndIndex
_windows_drive_kind_and_index(PyUnicodeObject *arg)
{
    return _windows_drive_kind_and_index_impl(PyUnicode_GET_LENGTH(arg), PyUnicode_KIND(arg), PyUnicode_DATA(arg));
}

WindowsDriveKind
_windows_drive_kind(PyUnicodeObject *arg)
{
    return _windows_drive_kind_and_index(arg).index;
}

Py_ssize_t
_windows_drive_index(PyUnicodeObject *arg)
{
    return _windows_drive_kind_and_index(arg).index;
}

PyObject *
windows_drive(PyObject *arg)
{
    PyUnicodeObject *fspath = _fspath(arg);
    if (!fspath)
    {
        return NULL;
    }
    // Don't need to normalize, normalization preserves drive.
    Py_ssize_t index = _windows_drive_index(fspath);
    if (index > 0)
    {
        PyObject *drive = PyUnicode_Substring((PyObject *)fspath, 0, index);
        Py_DECREF(fspath);
        return drive;
    }
    else
    {
        Py_DECREF(fspath);
        Py_RETURN_NONE;
    }
}

PyObject *windows_is_volume(PyObject *arg);
PyObject *windows_is_unc(PyObject *arg);
PyObject *windows_is_device_literal(PyObject *arg);
PyObject *windows_is_device_normalized(PyObject *arg);
PyObject *windows_is_device_unc(PyObject *arg);
